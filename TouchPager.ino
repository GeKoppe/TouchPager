#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#include "Arduino.h"
#include "Print.h"
#include "VKeys.h"

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4


Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
VKeys keyboard = VKeys("QWERTZ", YELLOW, BLACK, &tft);

void setup(void) {
    tft.reset();
    uint16_t identifier = tft.readID();

    Serial.begin(9600);
    if(identifier == 0x9325) {
      Serial.println(F("Found ILI9325 LCD driver"));
    } else if(identifier == 0x9328) {
      Serial.println(F("Found ILI9328 LCD driver"));
    } else if(identifier == 0x4535) {
      Serial.println(F("Found LGDP4535 LCD driver"));
    }else if(identifier == 0x7575) {
      Serial.println(F("Found HX8347G LCD driver"));
    } else if(identifier == 0x9341) {
      Serial.println(F("Found ILI9341 LCD driver"));
    } else if(identifier == 0x8357) {
      Serial.println(F("Found HX8357D LCD driver"));
    } else if(identifier==0x0101)
    {     
        identifier=0x9341;
        Serial.println(F("Found 0x9341 LCD driver"));
    }else {
      Serial.print(F("Unknown LCD driver chip: "));
      Serial.println(identifier, HEX);
      Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
      Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
      Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
      Serial.println(F("If using the breakout board, it should NOT be #defined!"));
      Serial.println(F("Also if using the breakout, double-check that all wiring"));
      Serial.println(F("matches the tutorial."));
      identifier=0x9341;
    
    }

    tft.begin(identifier);
    tft.fillScreen(BLACK);
    keyboard.init();

    pinMode(13, OUTPUT);
}

void loop() {
    unsigned long start = micros();
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z != 0) {
        Serial.println("Touch noticed!");
        keyboard.print(keyboard.getInputChar(p));
    }

    delay(10);
}

