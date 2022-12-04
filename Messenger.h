#ifndef messenger
#define messenger

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#include "VKeys.h"

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

class Messenger {
    public:
        Messenger(Elegoo_TFTLCD *screen, TouchScreen *ts, VKeys *keys);
        
        void setKeyColor(uint16_t color) { _keyColor = color; };
        uint16_t getKeyColor(void) { return _keyColor; };

        void setTextColor(uint16_t color) { _textColor = color; };
        uint16_t getTextColor(void) { return _textColor; };

        void setBackground(uint16_t color) { _background = color; };
        uint16_t getBackground(void) { return _background; };

    private:
        uint16_t _keyColor, _textColor, _background;
        VKeys *_keys;
        Elegoo_TFTLCD *_screen;
        TouchScreen *_ts;
};

#endif