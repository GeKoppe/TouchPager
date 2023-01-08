#ifndef messenger
#define messenger

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#include "VKeys.h"
#include "ColorChooser.h"
#include "Radio.h"

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFE

#define MAINMENU 0
#define WRITEMESSAGE 1
#define READ 2
#define OPTS 3

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

typedef struct Menu {
    int menuStart;
    int menuThickness;
    int menuOffset;
    String entries[5];
    String header;
    String extraText;
} Menu;

class Messenger {
    public:
        Messenger(Elegoo_TFTLCD *screen, TouchScreen *ts, VKeys *keys, Radio *r);

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

        byte adress[6];
        
        // PARTS
        VKeys *_keys;
        Elegoo_TFTLCD *_screen;
        TouchScreen *_ts;
        Radio *_radio;

        String _messages[3] = {
            "\0", "\0", "\0"
        };

        // MENU FUNCTIONS
        int mainMenu(void),
            getSelection(int menuStart, int menuThickness, int menuOffset, int entries, ScreenParse parse);
        
        uint16_t backGroundColorMenu(void);
        
        void    optsMenu(void),
                colorMenu(void),
                keysMenu(void),
                distanceMenu(void);

        String  keyStyleMenu(void);

        // READ
        void    readMenu(void),
                drawReadMenu(void),
                switchMessageToRead(int *msgCounter, bool plus, bool afterDelete);

        int readMenuSelection(ScreenParse parse);

        // MENU DRAW FUNCTIONS
        void drawMenu(Menu menu);

        // SETTERS
        void setKeyColor(uint16_t color) { _keyColor = (color == _background ? _keyColor : color); };
        void setTextColor(uint16_t color) { _textColor = (color == _background ? _keyColor : color); };
        void setTextSize(uint16_t size) { _textSize = size; };
        void setBoxColor(uint16_t color) { _boxColor = (color == _background ? _keyColor : color); };
        void setBackground(uint16_t color);

        // SPECIALS
        ScreenParse parseCoords(TSPoint p);
        
        String writeMessage(void);
        
        void    printMessageOnDisplay(String msg);
        
        String receiveMessage();

        // CACHE
        void    cacheMessage(String msg),
                clearMessageCache(),
                deleteMessage(int num);
        
        String  checkCache();
};

#endif