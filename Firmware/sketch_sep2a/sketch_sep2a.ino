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

int lastState[4] = {HIGH, HIGH, HIGH, HIGH};

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Booting...");

  pinMode(SWITCH_PIN_1, INPUT_PULLUP);
  pinMode(SWITCH_PIN_2, INPUT_PULLUP);
  pinMode(SWITCH_PIN_3, INPUT_PULLUP);
  pinMode(SWITCH_PIN_4, INPUT_PULLUP);
  
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
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);         
  tft.setCursor(0, 0);
  tft.print("Thing");
}

void loop() {
  int switch1State = digitalRead(SWITCH_PIN_1);
  int switch2State = digitalRead(SWITCH_PIN_2);
  int switch3State = digitalRead(SWITCH_PIN_3);
  int switch4State = digitalRead(SWITCH_PIN_4);

  int states[4] = {switch1State, switch2State, switch3State, switch4State};
  for (int i = 0; i < 4; i++) {
    if (states[i] != lastState[i]) {
      Serial.printf("Switch %d %s\n", i + 1, states[i] == LOW ? "PRESSED" : "released");
      lastState[i] = states[i];
    }
  }

  if (switch1State == LOW && switch4State == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.print("Hi");
    tone(BUZZER_PIN, 500);
    delay(300);
    noTone(BUZZER_PIN);
  }
  else if (switch1State == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.print("Switch 1 works");
    tone(BUZZER_PIN, 1000);
    delay(200);
    noTone(BUZZER_PIN);
  }
  else if (switch2State == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.print("Switch 2 works");
    tone(BUZZER_PIN, 500);
    delay(500);
    tone(BUZZER_PIN, 1000);
    delay(200);
    noTone(BUZZER_PIN);
  }
  else if (switch3State == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.print("Switch 3 works");
    delay(200);
    for (int i = 0; i < 5; i++) {
      tft.fillScreen(ST77XX_RED);
      tft.setCursor(0, 0);
      tft.print("Iteration " + String(i));
      // Syntax error corrected here:
      tone(BUZZER_PIN, 200 * i + 200); 
      delay(300);
      noTone(BUZZER_PIN);
    }
  }
  else if (switch4State == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.print("Switch 4 works");
    delay(200);
  }
}