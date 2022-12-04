
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
    _menuBorderOffset = 10;
    _minTouch = 10;

    _keys->setKeyColor(_keyColor);
}

void Messenger::init(void) {
    int selection = MAINMENU;
    String message;

    Serial.println("Screen Width: " + String(_screen->width()) + ", Screen Height: " + String(_screen->height()));

    while (true) {
        pinMode(A2, OUTPUT);
        pinMode(A3, OUTPUT);
        _screen->fillScreen(_background);

        switch (selection) {
            case MAINMENU: selection = mainMenu(); break;
            case WRITEMESSAGE: message = writeMessage(); selection = MAINMENU; break;
            default: break; 
        }
    }
}

String Messenger::writeMessage(void) {
    _keys->init();
    String msg = "";

    while (true) {
        
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        if (p.z > _minTouch) {
            msg += String(_keys->getInputChar(p));
            Serial.println(msg);
        }

        // Without delay, this thing notices touches 3-5 Times per Touch
        delay(25);
    }
}


/************************ PRIVATE ***************************/
int Messenger::showMenu(int menu) {
    int selection = 0;
    
}

int Messenger::getSelection(int menuStart, int menuThickness, int menuOffset, int entries, ScreenParse parse) {
    int x, y, selection;

    // Screen and touchscreen have different values, parse ts to screen
    // Get x and y coordinates from point
    x = (int) parse.x;
    y = (int) 320 - parse.y;

    Serial.println("X: " + String(x) + ", Y: " + String(y));

    // Check if touch occured out of menu
    if (y < menuStart || y > menuStart + (entries*menuThickness) + (entries*menuOffset)) return -1;
    if (x < _menuBorderOffset || x > (_screen->width() - _menuBorderOffset)) return -1;

    y -= menuStart;
    selection = (int) (y / menuThickness) + 1;
    selection = selection;

    if (selection < 1 || selection > entries) return -1;
    return selection;
}


int Messenger::mainMenu(void) {
    int menuStart = 100, menuThickness = 80, menuOffset = 20;

    drawMainMenu(menuStart, menuThickness, menuOffset);

    while (true) {
        int selection = -1;

        // Get touchpoint
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        // If touch was recognized
        if (p.z > _minTouch) {
            // Get selected menu poin
            selection = getSelection(menuStart, menuThickness, menuOffset, 2, parseCoords(p));

            Serial.println("Selection: " + String(selection));
            
            switch (selection) {
                case -1: break;
                case 1: return WRITEMESSAGE;
                default: break;
            }
        }
        delay(10);
    }
}

void Messenger::drawMainMenu(int menuStart, int menuThickness, int menuOffset) {
    // Print header
    _screen->setTextColor(RED);
    _screen->setTextSize(_textSize);
    _screen->setCursor(20,10);
    _screen->print("Messenger");
    
    // Print menu boxes
    _screen->drawRect(_menuBorderOffset, (int16_t) menuStart , _screen->width() - 2*_menuBorderOffset, (int16_t) menuThickness, _boxColor);
    _screen->drawRect(_menuBorderOffset, (int16_t) (menuStart + menuThickness + menuOffset) , _screen->width() - 2*_menuBorderOffset, (int16_t) menuThickness, _boxColor);

    // Print menu Texts
    _screen->setTextColor(_textColor);
    _screen->setCursor(20,120);
    _screen->print("Schreiben");

    _screen->setCursor(20,220);
    _screen->print("Optionen");
}

ScreenParse Messenger::parseCoords(TSPoint p) {
    ScreenParse parse;
    
    // Touch X: [120,940], Screen X: [0,240]
    // Parse X
    parse.x = (int) ((p.x - 120) / 3.5);

    // Touch Y: [70,920], Screen Y: [0,320]
    // Parse Y
    parse.y = (int) ((p.y - 70) / 2.66);
    return parse;
}