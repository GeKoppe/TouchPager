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

#define MAINMENU 0
#define STYLEMENU 1
#define WRITEMESSAGE 2

#define TEXTSMALL 2
#define TEXTMEDIUM 3
#define TEXTLARGE 4

typedef struct ScreenParse {
    int x;
    int y;
} ScreenParse;

class Messenger {
    public:
        Messenger(Elegoo_TFTLCD *screen, TouchScreen *ts, VKeys *keys);

        void init(void);
        
        void setKeyColor(uint16_t color) { _keyColor = color; };
        uint16_t getKeyColor(void) { return _keyColor; };

        void setTextColor(uint16_t color) { _textColor = color; };
        uint16_t getTextColor(void) { return _textColor; };

        void setTextSize(uint16_t size) { _textSize = size; };
        uint16_t getTextSize(void) { return _textSize; };

        void setBoxColor(uint16_t color) { _boxColor = color; };
        uint16_t getBoxColor(void) { return _boxColor; };

        void setBackground(uint16_t color) { _background = color; };
        uint16_t getBackground(void) { return _background; };

        String writeMessage(void);

    private:
        uint16_t _keyColor, _textColor, _background, _textSize, _boxColor;
        int _menuBorderOffset, _minTouch;
        VKeys *_keys;
        Elegoo_TFTLCD *_screen;
        TouchScreen *_ts;

        int showMenu(int menu), mainMenu(void), 
            getSelection(int menuStart, int menuThickness, int menuOffset, int entries, ScreenParse parse);

        void drawMainMenu(int menuStart, int menuThickness, int menuOffset);

        ScreenParse parseCoords(TSPoint p);
};

#endif