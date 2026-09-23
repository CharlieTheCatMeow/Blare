#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCLK 9
#define TFT_MOSI 10
#define TFT_RST 8
#define TFT_DC 4
#define TFT_CS 5
#define TFT_BL 6

#define SWITCH_PIN_1 0
#define SWITCH_PIN_2 1
#define SWITCH_PIN_3 2
#define SWITCH_PIN_4 3

#define BUZZER_PIN 7

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup() {
  Serial.begin(115200);

  pinMode(SWITCH_PIN_1, INPUT_PULLUP);
  pinMode(SWITCH_PIN_2, INPUT_PULLUP);
  pinMode(SWITCH_PIN_3, INPUT_PULLUP);
  pinMode(SWITCH_PIN_4, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);

  tft.init(76, 284); // Our panel size (portrait)
  //tft.setOffsets(82, 18); // Offsets for the weird resolution
  tft.invertDisplay(false); // Invert the colors (This display is flipped from normal)
  tft.setRotation(1); // Landscape, if it's upside down use 3!
  tft.fillScreen(ST77XX_BLACK); // clear the screen
  Serial.println("TFT Initialized!");
  tft.setCursor(0,0); // make the cursor at the top left

  tft.setTextSize(6);

  tft.print("Thing");
}

void loop() {
  int switch1State = digitalRead(SWITCH_PIN_1);
  int switch2State = digitalRead(SWITCH_PIN_2);
  int switch3State = digitalRead(SWITCH_PIN_3);
  int switch4State = digitalRead(SWITCH_PIN_4);

  if (switch1State == LOW) {
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
      tft.fillScreen(ST77XX_BLACK);
      tft.print("Iteration " + String(i));
      tone(BUZZER_PIN, 200 * i);
      delay(300);
      noTone(BUZZER_PIN);
    }
  }
  else if (switch4State == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.print("Switch 4 works");
    delay(200);
    if (switch1State == LOW && switch4State == LOW) {
      tft.fillScreen(ST77XX_BLACK);
      tft.print("Hi");
      tone(BUZZER_PIN, 500);
      delay(300);
    }
  }
}
