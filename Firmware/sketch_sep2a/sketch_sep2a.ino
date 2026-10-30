#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCLK 9    
#define TFT_MOSI 10   // SDA
#define TFT_RST  8 
#define TFT_DC   6    
#define TFT_CS   7    
#define TFT_BL   21   

#define SWITCH_PIN_1 2   
#define SWITCH_PIN_2 3   
#define SWITCH_PIN_3 4   
#define SWITCH_PIN_4 5   

#define BUZZER_PIN 20    

class Panel : public Adafruit_ST7789 {
public:
  using Adafruit_ST7789::Adafruit_ST7789;
  void setOffsets(int8_t col, int8_t row) { setColRowStart(col, row); }
};

// High-speed Hardware SPI
Panel tft = Panel(&SPI, TFT_CS, TFT_DC, TFT_RST);

const int BTN_MODE = 0;
const int BTN_SET = 1;
const int BTN_UP = 2;
const int BTN_DOWN = 3;

const uint8_t BUTTON_PINS[4] = {SWITCH_PIN_1, SWITCH_PIN_2, SWITCH_PIN_3, SWITCH_PIN_4};
const unsigned long DEBOUNCE_MS = 30;

bool rawState[4] = {false, false, false, false};
bool stableState[4] = {false, false, false, false};
unsigned long lastChangeMs[4] = {0, 0, 0, 0};
bool pressedEvent[4] = {false, false, false, false};

bool beepActive = false;
unsigned long beepStartMs = 0;
unsigned long beepDurationMs = 0;

void beep(int frequency, unsigned long durationMs) {
  tone(BUZZER_PIN, frequency);
  beepActive = true;
  beepStartMs = millis();
  beepDurationMs = durationMs;
}

void updateBeep() {
  if (beepActive && (millis() - beepStartMs >= beepDurationMs)) {
    noTone(BUZZER_PIN);
    beepActive = false;
  }
}

int hours = 12;
int minutes = 0;
int seconds = 0;
unsigned long lastTickMs = 0;

enum AppMode { MODE_CLOCK, MODE_SET_TIME };
AppMode currentMode = MODE_CLOCK;

int editHours = 0;
int editMinutes = 0;
int editField = 0;

bool displayDirty = true;

void updateButtons() {
  for (int i = 0; i < 4; i++) {
    pressedEvent[i] = false;

    bool pressedNow = (digitalRead(BUTTON_PINS[i]) == LOW);

    if (pressedNow != rawState[i]) {
      rawState[i] = pressedNow;
      lastChangeMs[i] = millis();
    }

    if ((millis() - lastChangeMs[i] >= DEBOUNCE_MS) && (rawState[i] != stableState[i])) {
      stableState[i] = rawState[i];
      if (stableState[i]) {
        pressedEvent[i] = true;
        beep(2000, 30);
        Serial.printf("Button %d pressed\n", i + 1);
      }
    }
  }
}

void updateClock() {
  while (millis() - lastTickMs >= 1000) {
    lastTickMs += 1000;
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
      }
    }
    if (currentMode == MODE_CLOCK) {
      displayDirty = true;
    }
  }
}

void enterSetTime() {
  editHours = hours;
  editMinutes = minutes;
  editField = 0;
  currentMode = MODE_SET_TIME;
  tft.fillScreen(ST77XX_BLACK);
  displayDirty = true;
}

void exitSetTime() {
  if (editHours != hours || editMinutes != minutes) {
    hours = editHours;
    minutes = editMinutes;
    seconds = 0;
    lastTickMs = millis();
  }
  currentMode = MODE_CLOCK;
  tft.fillScreen(ST77XX_BLACK);
  displayDirty = true;
  beep(1000, 150);
}

void handleClockMode() {
  if (pressedEvent[BTN_SET]) {
    enterSetTime();
  }
}

void handleSetTimeMode() {
  if (pressedEvent[BTN_MODE]) {
    editField = (editField + 1) % 2;
    displayDirty = true;
  }
  if (pressedEvent[BTN_UP]) {
    if (editField == 0) {
      editHours = (editHours + 1) % 24;
    } else {
      editMinutes = (editMinutes + 1) % 60;
    }
    displayDirty = true;
  }
  if (pressedEvent[BTN_DOWN]) {
    if (editField == 0) {
      editHours = (editHours + 23) % 24;
    } else {
      editMinutes = (editMinutes +59) % 60;
    }
    displayDirty = true;
  }
  if (pressedEvent[BTN_SET]) {
    exitSetTime();
  }
}

void drawTwoDigits(int x, int y, int value, uint16_t color) {
  char buf[3];
  snprintf(buf, sizeof(buf), "%02d", value);
  tft.setTextColor(color, ST77XX_BLACK);
  tft.setCursor(x, y);
  tft.print(buf);
}

void drawScreen() {
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.setCursor(0, 4);
  if (currentMode == MODE_CLOCK) {
    tft.print("TIME      ");
  } else if (editField == 0) {
    tft.print("SET HOUR  ");
  } else {
    tft.print("SET MINUTE");
  }

  int showH, showM, showS;
  uint16_t hourColor = ST77XX_WHITE;
  uint16_t minColor = ST77XX_WHITE;

  if (currentMode == MODE_CLOCK) {
    showH = hours;
    showM = minutes;
    showS = seconds;
  } else {
    showH = editHours;
    showM = editMinutes;
    showS = 0;
    if (editField == 0) {
      hourColor = ST77XX_YELLOW;
    } else {
      minColor = ST77XX_YELLOW;
    }
  }

  tft.setTextSize(5);
  const int Y = 30;
  drawTwoDigits(22, Y, showH, hourColor);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(82, Y);
  tft.print(":");
  drawTwoDigits(112, Y, showM, minColor);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(172, Y);
  tft.print(":");
  drawTwoDigits(202, Y, showS, ST77XX_WHITE);
}


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Booting...");

  for (int i = 0; i < 4; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TFT_BL, OUTPUT);
  
  // CRITICAL FIX: Pull to GND to activate the backlight LEDs
  digitalWrite(TFT_BL, LOW);

  // Pass -1 for CS so the Adafruit library can control it manually
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1);

  // Initialize display
  tft.init(76, 284); 
  tft.setOffsets(82, 18); 
  tft.invertDisplay(false); 
  tft.setRotation(1); 
  tft.fillScreen(ST77XX_BLACK); 
  Serial.println("TFT Initialized!");
  
  lastTickMs = millis();
}

void loop() {
  updateButtons();
  updateBeep();
  updateClock();

  switch (currentMode) {
    case MODE_CLOCK:    handleClockMode();    break;
    case MODE_SET_TIME: handleSetTimeMode();  break;
  }

  if (displayDirty) {
    drawScreen();
    displayDirty = false;
  }
}