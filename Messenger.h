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
#define WRITEMESSAGE 1
#define OPTS 2

#define COLOR_OPTS 21
#define CHANNEL_OPTS 22
#define TSIZE_OPTS 23
#define BACK_OPTS 24

#define TEXT_COLOR_OPTS 211
#define BG_COLOR_OPTS 212
#define KEY_COLOR_OPTS 213

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

        // MAIN FUNCTION
        void init(void);

        // RESET
        void reset(void);

        // GETTERS
        uint16_t getKeyColor(void) { return _keyColor; };
        uint16_t getTextColor(void) { return _textColor; };
        uint16_t getTextSize(void) { return _textSize; };
        uint16_t getBoxColor(void) { return _boxColor; };
        uint16_t getBackground(void) { return _background; };

    private:
        // CONFIG FIELDS
        uint16_t _keyColor, _textColor, _background, _textSize, _boxColor;
        int _menuBorderOffset, _minTouch;
        
        // PARTS
        VKeys *_keys;
        Elegoo_TFTLCD *_screen;
        TouchScreen *_ts;

        // MENU FUNCTIONS
        int mainMenu(void), 
            getSelection(int menuStart, int menuThickness, int menuOffset, int entries, ScreenParse parse);

        // MENU DRAW FUNCTIONS
        void    drawMainMenu(int menuStart, int menuThickness, int menuOffset);

        // SETTERS
        void setKeyColor(uint16_t color) { _keyColor = color; };
        void setTextColor(uint16_t color) { _textColor = color; };
        void setTextSize(uint16_t size) { _textSize = size; };
        void setBoxColor(uint16_t color) { _boxColor = color; };
        void setBackground(uint16_t color) { _background = color; };

        // SPECIALS
        ScreenParse parseCoords(TSPoint p);
        String writeMessage(void);
};

#endif