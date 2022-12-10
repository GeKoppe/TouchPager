
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
    String message = "";

    Serial.println("Screen Width: " + String(_screen->width()) + ", Screen Height: " + String(_screen->height()));

    while (true) {
        pinMode(A2, OUTPUT);
        pinMode(A3, OUTPUT);
        _screen->fillScreen(_background);

        switch (selection) {
            case MAINMENU: selection = mainMenu(); break;
            case WRITEMESSAGE: message = writeMessage(); selection = MAINMENU; break;
            case OPTS: optsMenu(); selection = MAINMENU; break;
            default: break; 
        }

        if (message != "") {
            // Nachricht versenden
        }
    }
}

void Messenger::reset(void) {
    _screen->fillScreen(WHITE);
    init();
}

/**
 * @brief 
 * 
 * @return String 
 */
String Messenger::writeMessage(void) {
    _keys->init();
    String msg = "", oldChar = "";
    TSPoint oldP = _ts->getPoint();
    int clicks = 0;

    while (true) {
        TSPoint p;

        do {
            digitalWrite(13, HIGH);
            p = _ts->getPoint();
            digitalWrite(13, LOW);
            
        } while (oldP.x == p.x && oldP.y == p.y);
        
        oldP.x = p.x;
        oldP.y = p.y;

        if (p.z > _minTouch) {
            String newChar = String(_keys->getInputChar(p));
            
            if (newChar == oldChar) {
                if (clicks < 9) {
                    clicks++;
                    continue;
                } else {
                    oldChar = "";
                    clicks = 0;
                }
            } else {
                msg += newChar;
                oldChar = newChar;
            }

            if (newChar == "*") {
                msg = msg.substring(0,msg.length()-1);
            }
            Serial.println(msg);
        }

        // Without delay, this thing notices touches 3-5 Times per Touch
        delay(10);
    }
}


/************************ PRIVATE ***************************/

/************************ PRIVATE MENU FUNCTIONS ***************************/

int Messenger::mainMenu(void) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    Menu menu;
    menu.menuStart = 100;
    menu.menuThickness = 80;
    menu.menuOffset = 20;
    menu.header = "Messenger";
    menu.entries[0] = String("Schreiben");
    menu.entries[1] = String("Optionen");
    menu.entries[2] = String("\0");
    menu.entries[3] = String("\0");
    menu.entries[4] = String("\0");

    drawMenu(menu);

    while (true) {
        int selection = -1;
        
        // Get touchpoint
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        // If touch was recognized
        if (p.z > _minTouch) {
            // Get selected menu poin
            selection = getSelection(menu.menuStart, menu.menuThickness, menu.menuOffset, 2, parseCoords(p));;

            Serial.println("Selection: " + String(selection));
            
            switch (selection) {
                case -1: break;
                case 1: return WRITEMESSAGE;
                case 2: return OPTS;
                default: break;
            }
        }
        delay(100);
    }
}

void Messenger::optsMenu(void) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    Menu menu;
    menu.menuStart = 60;
    menu.menuThickness = 40;
    menu.menuOffset = 20;
    menu.header = "Optionen";
    menu.entries[0] = String("Farben");
    menu.entries[1] = String("Kanaele");
    menu.entries[2] = String("Zurueck");
    menu.entries[3] = String("\0");
    menu.entries[4] = String("\0");

    drawMenu(menu);
    delay(100);

    while (true) {
        int selection = -1;
        
        // Get touchpoint
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        // If touch was recognized
        if (p.z > _minTouch) {
            // Get selected menu poin
            selection = getSelection(menu.menuStart, menu.menuThickness, menu.menuOffset, 3, parseCoords(p));

            Serial.println("Selection: " + String(selection));
            
            switch (selection) {
                case -1: break;
                case 1: colorMenu(); break;
                case 2: break;
                case 3: return;
                default: break;
            }
        }
        delay(100);
    }
}

void Messenger::colorMenu(void) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    Menu menu;
    menu.menuStart = 60;
    menu.menuThickness = 40;
    menu.menuOffset = 20;
    menu.header = "Farben";
    menu.entries[0] = String("Hintergrund");
    menu.entries[1] = String("Text");
    menu.entries[2] = String("Boxen");
    menu.entries[3] = String("Zurueck");
    menu.entries[4] = String("\0");

    drawMenu(menu);
    delay(100);

    while (true) {
        int selection = -1;
        
        // Get touchpoint
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        // If touch was recognized
        if (p.z > _minTouch) {
            // Get selected menu poin
            selection = getSelection(menu.menuStart, menu.menuThickness, menu.menuOffset, 4, parseCoords(p));

            Serial.println("Selection: " + String(selection));
            
            switch (selection) {
                case -1: break;
                case 1: break;
                case 2: break;
                case 3: break;
                case 4: return;
                default: break;
            }
        }
        delay(10);
    }
}

/************************ PRIVATE MENU DRAW ***************************/

void Messenger::drawMenu(Menu menu) {
    _screen->fillScreen(BLACK);
    // Print header
    _screen->setTextColor(RED);
    _screen->setTextSize(_textSize);
    _screen->setCursor(20,10);
    _screen->print(menu.header);

    _screen->setTextColor(_textColor);
    
    // Print menu boxes and texts
    for (int i = 0; i < 5; i++) {
        if (menu.entries[i] == "\0") continue;

        _screen->drawRect(_menuBorderOffset, 
            (int16_t) menu.menuStart + i*(menu.menuThickness + menu.menuOffset), 
            _screen->width() - _menuBorderOffset, 
            (int16_t) menu.menuThickness, 
            _boxColor
        );

        _screen->setCursor(20,menu.menuStart + i*(menu.menuThickness + menu.menuOffset) + ((int) (menu.menuThickness / _textSize)));
        _screen->print(menu.entries[i]);
    }
}

/************************ PRIVATE SPECIALS ***************************/
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

int Messenger::getSelection(int menuStart, int menuThickness, int menuOffset, int entries, ScreenParse parse) {
    int x, y, selection;

    // Screen and touchscreen have different values, parse ts to screen
    // Get x and y coordinates from point
    x = (int) parse.x;
    // Invert display
    y = (int) 320 - parse.y;

    Serial.println("X: " + String(x) + ", Y: " + String(y));

    // Check if touch occured out of menu
    if (y < menuStart || y > menuStart + (entries*menuThickness) + (entries*menuOffset)) return -1;
    if (x < _menuBorderOffset || x > (_screen->width() - _menuBorderOffset)) return -1;

    y -= menuStart;
    selection = (int) (y / (menuThickness+menuOffset)) + 1;
    selection = selection;

    if (selection < 1 || selection > entries) return -1;
    return selection;
}