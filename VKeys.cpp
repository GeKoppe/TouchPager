#include "VKeys.h"
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

/**
 * @brief Construct a new VKeys::VKeys object
 *  
 * @param style 
 * @param keyColor 
 * @param tScreen 
 */
VKeys::VKeys (String style, uint16_t keyColor, uint16_t textColor, Elegoo_TFTLCD *tScreen) {
    _style = style;
    _screen = tScreen;
    _keyColor = keyColor;
    _textColor = textColor;
    _textSize = 4;
    _special = false;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 10; j++) {
            if (_style == "QWERTZ") _rows[i][j] = _QWERTZ[i][j];
            else if (_style == "QWERTY") _rows[i][j] = _QWERTY[i][j];
            else if (_style == "ABCDE") _rows[i][j] = _ABCDE[i][j];
        }
    }
}

/**
 * @brief 
 * Initializes the keyboard on the touchscreen
 * 
 */
void VKeys::init (void) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    _screen->setTextColor(_textColor);
    _screen->setTextSize(_textSize - 1);

    // Spacebar, Done and Back
    _screen->fillRect(0, _screen->height() - (_kHeight - 1), (_screen->width() / 4) - 1, _kHeight - 1, _keyColor);
    _screen->fillRect((_screen->width() / 4), _screen->height() - (_kHeight - 1), 2*(_screen->width() / 4) - 1, _kHeight - 1, _keyColor);
    _screen->fillRect(3*(_screen->width() / 4), _screen->height() - (_kHeight - 1), (_screen->width() / 4) - 1, _kHeight - 1, _keyColor);
    _screen->setCursor((int16_t)(_screen->width() / 4), (int16_t)(_screen->height() - (_kHeight - 4)));

    _screen->setCursor((_screen->width() / 4) + 15, 291);    
    _screen->print((char*)"SPACE");

    _screen->setTextSize(_textSize - 2);
    _screen->setCursor(3*(_screen->width() / 4) + 8, 291);    
    _screen->print((char*)"Send");

    _screen->setCursor(8, 291);    
    _screen->print((char*)"Back");

    _screen->setTextSize(_textSize);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 10; j++) {
            _screen->fillRect(j*(_kWidth + 1), (int) _screen->height() - (4-i)*(_kHeight + 1), _kWidth, _kHeight, _keyColor);
            _screen->setCursor((int16_t)(j*(_kWidth + 1) +2), (int16_t)((int) _screen->height() - (4-i)*(_kHeight + 1) + 2));
            
            if (_special) {
                if (_specialChars[i][j] != '\0') {
                    _screen->print(_specialChars[i][j]);
                }
            } else {
                _screen->print(_rows[i][j]);
            }
        }
    }
}

/**
 * @brief 
 * Resets virtual keyboard. Useful if you want to change the color of the keys.
 */
void VKeys::reset (void) {
    // Default is QWERTZ
    if (_style == "QWERTY") {
        _rows[0][5] = 'Y';
        _rows[2][1] = 'Z';
    } else {
        _rows[0][5] = 'Z';
        _rows[2][1] = 'Y';
    }
}

/**
 * @brief 
 * 
 * @param point 
 * @return char 
 */
String VKeys::getInputChar(TSPoint point) {
    int16_t x,y;

    x = point.x;
    y = point.y;

    return String(getCharFromCoords(x, y));
}

void VKeys::setStyle (String style) { 
    if (style == "\0") return; 
    _style = style;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 10; j++) {
            if (_style == "QWERTZ") _rows[i][j] = _QWERTZ[i][j];
            else if (_style == "QWERTY") _rows[i][j] = _QWERTY[i][j];
            else if (_style == "ABCDE") _rows[i][j] = _ABCDE[i][j];
        }
    }
}

void VKeys::switchKeys() {
    _special = !_special;
    init();
}


/**************************************************** PRIVATE **************************************************/

String VKeys::getCharFromCoords(int16_t x, int16_t y) {
    int row, column;

    // Complete Y: [70,920], range 850, keyboard [70,475], keyboard range of 405, 4 rows so 405/4 = 101.25
    if (y > 475) return String('\0');
    row = (int) ((y - 70)/ 101.25);

    // row 0 is spacebar, therefore return escaped space immediately, as nothing else is there.
    if (row == 0) {
        int tempX = (int)((x - 120) / 3.5);

        if (tempX < (_screen->width() / 4)) {
            return String("BACK");
        } else if (tempX > 3*(_screen->width() / 4)) {
            return String("DONE");
        }
        return String(" ");
    }

    if (row > 3) return String('2');

    // X Range ca. 120 - 920 for 10 columns, therefore 800 / 10 = 80
    column = (int) ((x - 120)/ 80);

    if (column < 11 && column >= 0 && row < 4 && row >0) return String(_rows[(3-row)][column]);
    
    return String('-');
}