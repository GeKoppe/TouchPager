#ifndef colorchooser
#define colorchooser

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include "Messenger.h"

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

typedef struct Coords {
    int x;
    int y;
} Coords;

class ColorChooser {
    public:
        ColorChooser(Elegoo_TFTLCD *screen, TouchScreen *ts, uint16_t bg, uint16_t box, uint16_t text);

        int choose();
    private:
        uint16_t _bg, _box, _text;

        
        uint16_t _colors[8] = {
            BLUE,
            RED,
            GREEN,
            CYAN,
            MAGENTA,
            YELLOW,
            WHITE,
            BLACK
        };

        void draw();
        uint16_t getSelection(Coords coords);

        Elegoo_TFTLCD *_screen;
        TouchScreen *_ts;
};

#endif