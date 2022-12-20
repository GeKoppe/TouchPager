
#include "Messenger.h"

/**
 * @brief Construct a new Messenger:: Messenger object
 * 
 * @param screen Pointer to the LCD
 * @param ts Pointer to the touchscreen
 * @param keys pointer to the virtual keys
 */
Messenger::Messenger(Elegoo_TFTLCD *screen, TouchScreen *ts, VKeys *keys, Radio *r) {
    _screen = screen;
    _ts = ts;
    _keys = keys;
    _radio = r;

    // Set defaults
    _background = BLACK;
    _textColor = WHITE;
    _boxColor = WHITE;
    _keyColor = WHITE;
    _textSize = TEXTMEDIUM;
    _menuBorderOffset = 10;
    _minTouch = 3;

    // Set Defaults on the keys
    _keys->setKeyColor(_keyColor, _background);
    _keys->setTextColor(BLACK, _background);
}

/**
 * @brief 
 * Initializes the Messenger object and starts the infinite loop
 * 
 */
void Messenger::init(void) {
    int selection = MAINMENU;
    String message = ""; // Message to send

    Serial.println("Screen Width: " + String(_screen->width()) + ", Screen Height: " + String(_screen->height()));

    while (true) {
        // Pinmodes because TS and Screen share these pins
        pinMode(A2, OUTPUT);
        pinMode(A3, OUTPUT);
        _screen->fillScreen(_background);

        // Start the corresponding menu
        switch (selection) {
            case MAINMENU: selection = mainMenu(); break;
            case WRITEMESSAGE: message = writeMessage(); selection = MAINMENU; break;
            case OPTS: optsMenu(); selection = MAINMENU; break;
            case READ: readMenu(); selection = MAINMENU; break;
            default: selection = mainMenu(); 
        }

        if (message != "") {
            // TODO get real message here and send it, this stuff is just for testing!
            _radio->sendMessage(message);
            message = "";
        }
    }
}

/**
 * @deprecated
 * @brief 
 * 
 */
void Messenger::reset(void) {
    _screen->fillScreen(WHITE);
    init();
}



/************************ PRIVATE ***************************/

/************************ PRIVATE MENU FUNCTIONS ***************************/

/**
 * @brief
 * Shows the main menu and handles the input
 * 
 * @return int 
 */
int Messenger::mainMenu(void) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);

    // Build menu defintion
    Menu menu;
    menu.menuStart = 100;
    menu.menuThickness = 60;
    menu.menuOffset = 10;
    menu.header = "Messenger";
    menu.extraText = checkCache();
    menu.entries[0] = String("Schreiben");
    menu.entries[1] = String("Lesen");
    menu.entries[2] = String("Optionen");
    menu.entries[3] = String("\0");
    menu.entries[4] = String("\0");

    // Draw menu
    drawMenu(menu);

    while (true) {
        String inc = receiveMessage();

        if (inc != "\0") {
            cacheMessage(inc);
            return MAINMENU;
        }
        int selection = -1;
        
        // Get touchpoint
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        // If touch was recognized
        if (p.z > _minTouch) {
            // Get selected menu poin
            selection = getSelection(menu.menuStart, menu.menuThickness, menu.menuOffset, 3, parseCoords(p));;

            Serial.println("Selection: " + String(selection));
            
            // Return the seection
            switch (selection) {
                case -1: break;
                case 1: return WRITEMESSAGE;
                case 2: return READ;
                case 3: return OPTS;
                default: break;
            }
        }
        delay(10);
    }

    Serial.println("Returning from mainMenu");
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
    menu.entries[2] = String("Tastatur");
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
                case 1: colorMenu(); return;
                case 2: break;
                case 3: keysMenu(); return;
                case 4: return;
                default: break;
            }
        }
        delay(10);
    }

    Serial.println("Returning from optsMenu");
}

void Messenger::keysMenu(void) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    Menu menu;
    menu.menuStart = 60;
    menu.menuThickness = 40;
    menu.menuOffset = 20;
    menu.header = "Tastatur";
    menu.entries[0] = String("Tastenfarbe");
    menu.entries[1] = String("Buchs.-Far.");
    menu.entries[2] = String("Stil");
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

            ColorChooser cc(_screen, _ts, _background, _boxColor, _textColor);
            
            switch (selection) {
                case -1: break;
                case 1: _keys->setKeyColor(cc.choose(), _background); return;
                case 2: _keys->setTextColor(cc.choose(), _background); break;
                case 3: _keys->setStyle(keyStyleMenu());
                case 4: return;
                default: break;
            }
        }
        delay(10);
    }

    Serial.println("Returning from optsMenu");
}

String Messenger::keyStyleMenu(void) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    Menu menu;
    menu.menuStart = 60;
    menu.menuThickness = 40;
    menu.menuOffset = 20;
    menu.header = "Tastatur";
    menu.entries[0] = String("QWERTZ");
    menu.entries[1] = String("QWERTY");
    menu.entries[2] = String("ABCDE");
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
                case 1: return String("QWERTZ");
                case 2: return String("QWERTY");
                case 3: return String("ABCDE");
                case 4: return String("\0");
                default: break;
            }
        }
        delay(10);
    }

    Serial.println("Returning from optsMenu");
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
            
            ColorChooser cc(_screen, _ts, _background, _boxColor, _textColor);

            switch (selection) {
                case -1: break;
                case 1: setBackground(backGroundColorMenu()); return;
                case 2: setTextColor(cc.choose()); return;
                case 3: setBoxColor(cc.choose()); return;
                case 4: return;
                default: break;
            }
        }
        delay(10);
    }
    Serial.println("Returning from colorMenu");
}

uint16_t Messenger::backGroundColorMenu(void) {
    Menu menu;
    menu.menuStart = 60;
    menu.menuThickness = 70;
    menu.menuOffset = 20;
    menu.header = "Hintergrund";
    menu.entries[0] = String("Dark");
    menu.entries[1] = String("Light");
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
                case 1: return BLACK;
                case 2: return WHITE; 
                case 3: return 0;
                default: break;
            }
        }
        delay(10);
    }

    Serial.println("Returning from bgMenu");
}

/************************ PRIVATE MENU DRAW ***************************/

/**
 * @brief 
 * Draws the menu from definition passed in argument
 * 
 * @param menu Definition of the menu
 */
void Messenger::drawMenu(Menu menu) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    _screen->fillScreen(_background);
    
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
            _screen->width() - 2*_menuBorderOffset, 
            (int16_t) menu.menuThickness, 
            _boxColor
        );

        _screen->setCursor(20,menu.menuStart + i*(menu.menuThickness + menu.menuOffset) + ((int) (menu.menuThickness / _textSize)));
        _screen->print(menu.entries[i]);
    }

    // Print extra text, if it exists
    if (menu.extraText != "") {
        int y = (10 + menu.menuStart) / 2;
        _screen->setTextSize(_textSize - 1);
        _screen->setTextColor(_textColor);
        _screen->setCursor(20,y);
        _screen->print(menu.extraText);
    }
}

/****************************** PRIVATE READ MENU STUFF ****************************/

/**
 * @brief 
 * Menu for reading received messages
 * 
 */
void Messenger::readMenu(void) {
    int currentMessage = 0;

    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    
    drawReadMenu();

    _screen->setCursor(0,10);

    _screen->setTextSize(_textSize);
    _screen->setTextColor(_textColor);

    // If there are no new messages, return
    if (checkCache() == "") {
        _screen->print("Keine neuen\nNachrichten");
        delay(3000);
        return;
    }

    // Print first message
    _screen->print(_messages[currentMessage]);
    while (true) {
        int selection = -1;
        
        // Get touchpoint
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        if (p.z > _minTouch) {
            // Get selected menu poin
            // selection = getSelection(menu.menuStart, menu.menuThickness, menu.menuOffset, 3, parseCoords(p));
            selection = readMenuSelection(parseCoords(p));

            Serial.println("Selection: " + String(selection));
            
            switch (selection) {
                case -1: break;
                case 1: switchMessageToRead(&currentMessage, false, false); break;
                case 2: switchMessageToRead(&currentMessage, true, false); break;
                case 3: deleteMessage(currentMessage); switchMessageToRead(&currentMessage, false, true); break;
                case 4: return;
                default: break;
            }
            delay(10);
        }

        continue;
    }
}

void Messenger::drawReadMenu(void) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    _screen->fillScreen(_background);
    _screen->setTextSize(_textSize -1);
    _screen->setTextColor(_textColor);

    _screen->drawRect(_menuBorderOffset, 180, 100, 50, _boxColor);
    _screen->drawRect(_screen->width() - _menuBorderOffset - 100, 180, 100, 50, _boxColor);
    _screen->drawRect(_menuBorderOffset, 240, 100, 50, _boxColor);
    _screen->drawRect(_screen->width() - _menuBorderOffset - 100, 240, 100, 50, _boxColor);

    _screen->setCursor(_menuBorderOffset + 40, 195);
    _screen->print("<");

    _screen->setCursor(_screen->width() - _menuBorderOffset - 100 + 48, 195);
    _screen->print(">");

    _screen->setCursor(_menuBorderOffset + 10, 255);
    _screen->print("DEL");

    _screen->setCursor(_menuBorderOffset + 140, 255);
    _screen->print("BACK");
}

void Messenger::switchMessageToRead(int *msgCounter, bool plus, bool afterDelete) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    _screen->fillRect(0,0,_screen->width(), 170, _background);
    _screen->setCursor(0,0);
    _screen->setTextColor(_textColor);
    _screen->setTextSize(_textSize);

    int tempCounter = *msgCounter;
    int noMessages = 0;
    plus ? tempCounter++ : tempCounter--;

    for (int i = 0; i < 3; i++) {
        if (String(_messages[i]) != String("\0")) noMessages++;
    }

    if (noMessages == 3) {
        *msgCounter = 0;
        _screen->print("Keine neuen\nNachrichten");
        return;
    }

    if (afterDelete) {
        for (int i = 0; i < 3; i++) {
            if (String(_messages[i]) == String("\0")) {
                tempCounter = i-1; 
                if (tempCounter > 2 || tempCounter < 0) tempCounter = 0;
                break; 
            }
        }
    }

    if (tempCounter > 2 || tempCounter < 0) return;

    if (String(_messages[tempCounter]) == String("\0")) return;

    *msgCounter = tempCounter;
    _screen->print(_messages[*msgCounter]);
}

int Messenger::readMenuSelection(ScreenParse parse) {
    int selection = 1;
    int y = 320 - parse.y;

    Serial.println("Y: " + String(y) + "; parse.y: " + String(parse.y));

    if (y < 180) return -1;
    if (y > 290) return -1;
    if (parse.x < _menuBorderOffset) return -1;
    if (parse.x > _screen->width() - _menuBorderOffset) return -1;

    selection += (y > 235 ? 2 : 0);
    selection += (parse.x > (_screen->width() / 2) ? 1 : 0);

    return selection;
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

/**
 * @brief 
 * 
 * @param color 
 */
void Messenger::setBackground(uint16_t color) {
    if (color == _textColor) _textColor = (color == WHITE ? BLACK : WHITE);
    if (color == _boxColor) _boxColor = (color == WHITE ? BLACK : WHITE);

    _background = color;
}

/**
 * @brief Prints the current message to the display
 * 
 * @param msg {String} Current message 
 */
void Messenger::printMessageOnDisplay(String msg) {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    _screen->fillRect(0,0,_screen->width(), 90, _background);
    _screen->setCursor(0,0);
    _screen->setTextSize(_textSize);
    _screen->setTextColor(_textColor);
    _screen->print(msg);
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
            bool hasChanged = false;
            String newChar = String(_keys->getInputChar(p));
            
            if (newChar == oldChar) {
                if (clicks < 5) {
                    clicks++;
                    continue;
                } else {
                    oldChar = "";
                    newChar = "";
                    clicks = 0;
                }
            } else {
                if (newChar != "~" && newChar != "s" && newChar != "DONE" && newChar != "BACK") {
                    msg += newChar;
                }
                oldChar = newChar;
                hasChanged = true;
            }

            if (newChar == "~") {
                msg = msg.substring(0,msg.length()-2);
                printMessageOnDisplay(msg);
            } else if (newChar == "s") {
                _keys->switchKeys();
            } else if (newChar == "DONE") {
                break;
            } else if (newChar == "BACK") {
                msg = "";
                break;
            } else {
                if (hasChanged) {
                    printMessageOnDisplay(msg);
                }
            }

            Serial.println(msg);
        }

        // Without delay, this thing notices touches 3-5 Times per Touch
        delay(10);
    }

    return msg;
}

/**
 * @brief 
 * 
 * @return String 
 */
String Messenger::receiveMessage() {
    String msg = "\0";
    // String msg = _radio->receiveMessage();
    if (msg == "\0") return "\0";

    cacheMessage(msg);
    return msg;
}

/********************************** PRIVATE CACHE METHODS ********************************************/

/**
 * @brief 
 * Caches incoming messages in message cache
 * 
 * @param msg String to be cached
 */
void Messenger::cacheMessage(String msg) {
    // Check, how many messages are in cache
    int counter = 0;
    for (int i = 0; i < 3; i++) {
        if (_messages[i] != "\0") {
            counter++;
            continue;
        } else {
            break;
        }
    }

    // If there are less than 3 messages, just cache the newest message on last position
    if (counter < 3) {
        _messages[counter] = msg;
        return;
    } else {
        // If there are 3 messages, clear the last message
        _messages[2] = "\0";

        // Push every other message one to the back
        for (int i = 0; i < 2; i++) {
            _messages[i + 1] = _messages[i];
        }

        // Put new Message as first in the array
        _messages[0] = msg;
        return;
    }
}

/**
 * @brief
 * Clears the whole message cache. Every message is deleted!
 * 
 */
void Messenger::clearMessageCache() {
    for (int i = 0; i < 3; i++) {
        _messages[i] = "\0";
    }
}

/**
 * @brief 
 * Deletes a certain message from cache.
 * 
 * @param num Number of message to be deleted. Has to be between 0 and 2!
 */
void Messenger::deleteMessage(int num) {
    if (num < 0 || num > 2) return;

    // Delete requested message from cache
    _messages[num] = String("\0");

    // For every message, that is listed after the requested message: move upwards in array
    for (int i = num; i < 3; i++) {
        if (i != 2) {
            if (_messages[i + 1] != String("\0")) {
                _messages[i] = String(_messages[i+1]);
                _messages[i+1] = String("\0");
            }
        }
    }
}

/**
 * @brief 
 * Checks, whether there are messages in cache and returns a String for the main Menu screen.
 * 
 * @return String 
 */
String Messenger::checkCache() {
    // Counter for the number of messages
    int number = 0;

    // Iterate through all messages
    for (int i = 0; i < 3; i++) {
        // Increment counter for every message, that isn't \0
        if (String(_messages[i]) != String("\0")) {
            Serial.println("Cache Analysis, Message " + String(i) + ": " + String(_messages[i]));
            number++;
        }
    }

    if (number == 0) return "";

    // Return message
    return String(number) + String(" Nachrichten");
}