
#include "Messenger.h"

/**
 * @brief Construct a new Messenger:: Messenger object
 * 
 * @param screen 
 * @param ts 
 * @param keys 
 */
Messenger::Messenger(Elegoo_TFTLCD *screen, TouchScreen *ts, VKeys *keys) {
    _screen = screen;
    _ts = ts;
    _keys = keys;

    _background = BLACK;
    _textColor = WHITE;
    _boxColor = WHITE;
    _keyColor = BLUE;
    _textSize = TEXTMEDIUM;

    _keys->setKeyColor(_keyColor);
}

void Messenger::init(void) {
    _screen->fillScreen(_background);
    mainMenu();
}

String Messenger::writeMessage(void) {

}


/************************ PRIVATE ***************************/
int Messenger::showMenu(int menu) {
    int selection = 0;
    
}


int Messenger::mainMenu(void) {
    // Reset screen
    _screen->fillScreen(_background);

    // Print header
    _screen->setTextColor(RED);
    _screen->setTextSize(_textSize);
    _screen->setCursor(20,10);
    _screen->print("Messenger");
    
    // Print menu boxes
    _screen->drawRect(10, (int16_t) 100 , _screen->width() - 20, (int16_t) (_screen->height() / 5), _boxColor);
    _screen->drawRect(10, (int16_t) 200 , _screen->width() - 20, (int16_t) (_screen->height() / 5), _boxColor);

    // Print menu Texts
    _screen->setTextColor(_textColor);
    _screen->setCursor(20,120);
    _screen->print("Schreiben");

    _screen->setCursor(20,220);
    _screen->print("Optionen");

    while (true) {
        continue;
    }
}