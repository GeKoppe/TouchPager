#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

// #include "Arduino.h"
// #include "Print.h"
#include "VKeys.h"
#include "Messenger.h"
#include "ColorChooser.h"
#include "Radio.h"


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
VKeys keyboard = VKeys("QWERTZ", WHITE, BLACK, &tft);
Radio radio = Radio(43, 42);
Messenger msg = Messenger(&tft, &ts, &keyboard, &radio);


void setup(void) {
    radio.init();
    tft.reset();
    uint16_t identifier = tft.readID();
    Serial.begin(9600);
    Serial.println("We in Radio Constructor");
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
      Serial.println(identifier, HEX);
      identifier=0x9341;
    
    }

    tft.begin(identifier);

    pinMode(13, OUTPUT);
}

void loop() {
    Serial.println("Kinda started again");
    radio.init();
    radio.sendMessage(String("Test1234"));

    delay(5000);
}

