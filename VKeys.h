#ifndef vkeys
#define vkeys

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


class VKeys {
    public:
        VKeys (String style, uint16_t keyColor, uint16_t textColor, Elegoo_TFTLCD *tScreen);
        
        void setKWidth (int width) { _kWidth = width; };
        int getKWidth () { return _kWidth; };

        void setKHeight (int height) { _kHeight = height; };
        int getKHeight () { return _kHeight; };

        void setStyle (String style) { _style = style; };
        String getStyle () { return _style; };

        void setKeyColor (uint16_t color) { _keyColor = color; };
        uint16_t getKeyColor (void) { return _keyColor; };

        void setTextColor (uint16_t color) { _textColor = color; };
        uint16_t getTextColor (void) { return _textColor; };

        void    init (void),
                reset (void);
        
        String getInputChar(TSPoint point);
        void print(String msg);

    private:
        String _style;
        uint16_t _keyColor;
        uint16_t _textColor;
        int _textSize = 1;

        int _kHeight = 40 - 2;
        int _screenX = 240;
        int _kWidth = (_screenX / 10) - 1;
        int _screenY = 300;

        char _rows[3][10] = {
            {
                'Q','W','E','R','T','Z','U','I','O','P'
            },
            {
                'A','S','D','F','G','H','J','K','L','#'
            },
            {
                ',','Y','X','C','V','B','N','M','?','*'
            }
        };

        String getCharFromCoords (int16_t x, int16_t y);

        Elegoo_TFTLCD *_screen;
};

#endif