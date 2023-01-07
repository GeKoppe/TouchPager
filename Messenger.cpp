
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
 * All menu functions work the exact same, therefore not all of them are commented. This comment
 * shows the function of menu methods
 * 
 *  1. Configure the menu with a menu struct
 *      1.1 menuStart: Y-Value where the menu starts on display
 *      1.2 menuThickness: How thick are the menu entries
 *      1.3 menuOffset: how far are entries apart
 *      1.4 header: Header text of the menu
 *      1.5 extraText: smaller text below menu header
 *      1.6 entries[5]: Text shown in the menu entries
 *  2. Draw the menu
 *  3. Infinite loop
 *      3.1 Check, if messages are available on the air
 *      3.2 Get touchpoint
 *      3.3 analyse touchpoint
 *      3.4 Call method or return value according to selection and break
 */

/**
 * @brief
 * Shows the main menu and handles the input
 * 
 * @return int Selection in menu
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
            Serial.println(inc);
            cacheMessage(inc);
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

/**
 * @brief 
 * Options menu
 * 
 */
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

/**
 * @brief
 * Submenu for all things keyboard related
 * 
 */
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

/**
 * @brief 
 * Menu for selecting layout of keys
 * 
 * @return String Layout of the keys
 */
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

/**
 * @brief 
 * Submenu for different color menus
 * 
 */
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

/**
 * @brief 
 * Menu to set either light or dark mode
 * 
 * @return uint16_t WHITE or BLACK
 */
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
 */
void Messenger::readMenu(void) {
    // Couunter for which message screen is on
    int currentMessage = 0;

    // Set pinmodes
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    
    // Draw the menu
    drawReadMenu();

    // Set text config
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
                case 1: switchMessageToRead(&currentMessage, false, false); break; // Message back
                case 2: switchMessageToRead(&currentMessage, true, false); break; // Message forward
                case 3: deleteMessage(currentMessage); switchMessageToRead(&currentMessage, false, true); break; // Delete current
                case 4: return;
                default: break;
            }
            delay(10);
        }

        continue;
    }
}

/**
 * @brief 
 * Method for drawing the read menu on screen
 */
void Messenger::drawReadMenu(void) {
    // Set pinmodes
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);

    // Fill screen and set text config
    _screen->fillScreen(_background);
    _screen->setTextSize(_textSize -1);
    _screen->setTextColor(_textColor);

    // Draw function buttons
    _screen->drawRect(_menuBorderOffset, 180, 100, 50, _boxColor);
    _screen->drawRect(_screen->width() - _menuBorderOffset - 100, 180, 100, 50, _boxColor);
    _screen->drawRect(_menuBorderOffset, 240, 100, 50, _boxColor);
    _screen->drawRect(_screen->width() - _menuBorderOffset - 100, 240, 100, 50, _boxColor);

    // Fill function buttons
    _screen->setCursor(_menuBorderOffset + 40, 195);
    _screen->print("<");

    _screen->setCursor(_screen->width() - _menuBorderOffset - 100 + 48, 195);
    _screen->print(">");

    _screen->setCursor(_menuBorderOffset + 10, 255);
    _screen->print("DEL");

    _screen->setCursor(_menuBorderOffset + 140, 255);
    _screen->print("BACK");
}

/**
 * @brief 
 * Writes new message to display, after it has been switched
 * 
 * @param msgCounter Pointer to the current messagenumber
 * @param plus Message +1 or -1?
 * @param afterDelete Triggered after a message has been deleted?
 */
void Messenger::switchMessageToRead(int *msgCounter, bool plus, bool afterDelete) {
    // Set pinmodes
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);

    // Overprint old message and set text configuration
    _screen->fillRect(0,0,_screen->width(), 170, _background);
    _screen->setCursor(0,0);
    _screen->setTextColor(_textColor);
    _screen->setTextSize(_textSize);

    // Set tempCounter for later comparison
    int tempCounter = *msgCounter;
    int noMessages = 0;

    // Modify tempcounter
    plus ? tempCounter++ : tempCounter--;

    // Check, how many empty messages are in cache
    for (int i = 0; i < 3; i++) {
        if (String(_messages[i]) != String("\0")) noMessages++;
    }

    // If message cache is empty, set msgCounter back to 0 and print, that there are no messages
    if (noMessages == 3) {
        *msgCounter = 0;
        _screen->print("Keine neuen\nNachrichten");
        return;
    }

    // If method was called after deletion, do it a little differently
    if (afterDelete) {
        for (int i = 0; i < 3; i++) {
            if (String(_messages[i]) == String("\0")) {
                tempCounter = i-1; 
                if (tempCounter > 2 || tempCounter < 0) tempCounter = 0;
                break; 
            }
        }
    }

    // Handle tempcounter being outside of array indices
    if (tempCounter > 2 || tempCounter < 0) return;

    if (String(_messages[tempCounter]) == String("\0")) return;

    // Set msg counter and print message
    *msgCounter = tempCounter;
    _screen->print(_messages[*msgCounter]);
}

/**
 * @brief 
 * Gets selection in the read menu
 * 
 * @param parse x and y coordinates of pressed point
 * @return int 1: top left, 2: top right, 3: bottom left, 4: bottom right
 */
int Messenger::readMenuSelection(ScreenParse parse) {
    int selection = 1;
    int y = 320 - parse.y;

    Serial.println("Y: " + String(y) + "; parse.y: " + String(parse.y));

    // Check, if point is outside of menu
    if (y < 180) return -1;
    if (y > 290) return -1;
    if (parse.x < _menuBorderOffset) return -1;
    if (parse.x > _screen->width() - _menuBorderOffset) return -1;

    // Get row and add 2, if second row. Works because of numbering of menu entries
    selection += (y > 235 ? 2 : 0);

    // Get column and add 1, if second column
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

/**
 * @brief 
 * Gets number of menupoint selected
 * 
 * @param menuStart y-Value, where the menu starts
 * @param menuThickness how thick one menupoint is
 * @param menuOffset how far the menu entries are apart
 * @param entries number of entries
 * @param parse x and y values of the pressed point
 * @return int number of the menu entry selected
 */
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

    // translate y to the start of the menu and get selection
    y -= menuStart;
    selection = (int) (y / (menuThickness+menuOffset)) + 1;
    selection = selection;

    if (selection < 1 || selection > entries) return -1;
    return selection;
}

/**
 * @brief 
 * Sets private field _background to color that you want
 * 
 * @param color Color you want to set the background to
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
    // Set pinmodes
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);

    // Print a filled rect over old message
    _screen->fillRect(0,0,_screen->width(), 90, _background);
    
    // Set config values for text and print it
    _screen->setCursor(0,0);
    _screen->setTextSize(_textSize);
    _screen->setTextColor(_textColor);
    _screen->print(msg);
}


/**
 * @brief 
 * Function to write a message that you want to send
 * 
 * @return String Written message
 */
String Messenger::writeMessage(void) {
    // Initialize the keypad (draw it on screen)
    _keys->init();

    // Initialize variables and get initial touchpoint
    String msg = "", oldChar = "";
    TSPoint oldP = _ts->getPoint();
    
    // Clicks: Counter for how long since current char has been pressed. It works, I forgot how
    int clicks = 0;

    while (true) {
        TSPoint p;

        do {
            // Get touchpoint, until the point differs from last touched point on display
            digitalWrite(13, HIGH);
            p = _ts->getPoint();
            digitalWrite(13, LOW);     
        } while (oldP.x == p.x && oldP.y == p.y);
        
        // Save new point in old point for later comparison
        oldP.x = p.x;
        oldP.y = p.y;

        if (p.z > _minTouch) {
            // hasChanged: records, whether the new char differs from last char
            bool hasChanged = false;

            // Get character from pressed point
            String newChar = String(_keys->getInputChar(p));
            
            // Check, if pressed char differs from last pressed char
            if (newChar == oldChar) {
                // if they don't differ, count how many times we have been at this point
                if (clicks < 3) {
                    clicks++;
                    continue;
                } else {
                    // else reset all variables
                    oldChar = "";
                    newChar = "";
                    clicks = 0;
                }
            } else {
                // Unless a special key was pressed, add new char to message
                if (newChar != "~" && newChar != "s" && newChar != "DONE" && newChar != "BACK") {
                    msg += newChar;
                }

                // record, that something has changed 
                oldChar = newChar;
                hasChanged = true;
            }

            // If delete was pressed, delete last char from message
            if (newChar == "~") {
                msg = msg.substring(0,msg.length()-2);
                printMessageOnDisplay(msg);
            } else if (newChar == "s") {
                // small s means, that we want to switch to special keys (numbers and non-alpha-numeric)
                _keys->switchKeys();
            } else if (newChar == "DONE") {
                // If done, break infinite loop to return the message
                break;
            } else if (newChar == "BACK") {
                // If cancelled, reset message and break infinite loop
                msg = "";
                break;
            } else {
                if (hasChanged) {
                    // If the input char has changed, print message to display
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
 * Gets message that was sent from other device
 * 
 * @return String received message
 */
String Messenger::receiveMessage() {
    // Try to get message and return, if message is \0
    String msg = _radio->receiveMessage();
    if (msg == "\0") return "\0";

    // Cache that message and return it
    Serial.println("Received message: " + String(msg));
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