#include "ColorChooser.h"

/**
 * @brief Construct a new Color Chooser:: Color Chooser object
 * 
 * @param screen Pointer to the screen object to be used
 * @param ts Pointer to the touchscreen object to be used
 * @param bg Current background color
 * @param box Current box color
 * @param text Current text color
 */
ColorChooser::ColorChooser(Elegoo_TFTLCD *screen, TouchScreen *ts, uint16_t bg, uint16_t box, uint16_t text) {
    _screen = screen;
    _ts = ts;
    _bg = bg;
    _box = box;
    _text = text;
}

/**
 * @brief 
 * Draws the color chose menu on the display and let's the user choose one of them by clicking it.
 * 
 * @return int Chosen color
 */
int ColorChooser::choose() {
    // Set variable for selection
    uint16_t selection = -1;

    // Draw the menu to the screen
    draw();

    // Wait for input
    while (true) {
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        // If there was a user input, analyse it.
        if (p.z > 20) {
            // Parse coordinates from touchscreen to screen
            Coords parse;
            parse.x = (int) ((p.x - 120) / 3.5);
            parse.y = (int) ((p.y - 70) / 2.66);

            // Get selection from parsed coordinates
            selection = getSelection(parse);

            // If valid selection occured, break
            if (selection != -2) break;
        }

        // Delay, in order not to get the same input more than once. 
        delay(5);
    }

    // Return the selection
    return selection;
}

/**
 * @brief 
 * Analyses coordinates passed into function and returns selected color
 * 
 * @param coords Coordinates of touchpoint, parsed to screen coordinates
 * @return uint16_t chosen Color or 0, if cancelled or -2, if selection was not valid
 */
uint16_t ColorChooser::getSelection(Coords coords) {
    int column, row;

    Serial.println("CC: X: " + String(coords.x) + ", Y: " + String(coords.y));

    // Check, if touchpoint is at height of cancel button
    if (320 - coords.y > 250) return 0;

    // If coordinates are out of valid ranges, return failure marker
    if (coords.x < 35 || coords.x > 215) return -2;
    if (coords.x < 135 && coords.x > 100) return -2;

    // If x is less than 100, it has to be first column (index 0), otherwise second column (index 1)
    column = coords.x < 100 ? 0 : 1;

    // Invert coordinates (320-y, because screen is 320 high) and divide by 60, because that's the 
    // height of the boxes, to get the row
    row = (int)((320 - coords.y) / 60);

    // Variable basically only exists, so that we don't try to reach indices the array does not have
    // This works, because the first column has indices 0,2,4,6 and second has 1,3,5,7
    int safeHandler = column + (row*2);

    // If selection > 7, set selection to failure, otherwise set selection to selected color
    uint16_t selection = (safeHandler > 7 ? -2 : _colors[safeHandler]);

    switch (selection) {
        case BLUE: Serial.println("CC Selection: BLUE"); break;
        case RED: Serial.println("CC Selection: RED"); break;
        case GREEN: Serial.println("CC Selection: GREEN"); break;
        case CYAN: Serial.println("CC Selection: CYAN"); break;
        case MAGENTA: Serial.println("CC Selection: MAGENTA"); break;
        case YELLOW: Serial.println("CC Selection: YELLOW"); break;
        case WHITE: Serial.println("CC Selection: WHITE"); break;
        case BLACK: Serial.println("CC Selection: BLACK"); break;
        default: Serial.println("CC No selection"); break;
    }

    return selection;
}

/**
 * @brief 
 * Draws the menu to the screen
 * 
 */
void ColorChooser::draw() {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    _screen->fillScreen(_bg);

    // Box Size: 70. Boxes start at y = 10 and end at y = 240, box offset 10
    // Draw boxes, fill with corresponding color
    for (int i = 0; i < 8; i++) {
        _screen->fillRect((i % 2 == 0 ? 35 : 135),
            ((int) (i / 2))*50 + 10,
            70,
            50,
            _colors[i]
        );
    }

    // Draw cancel button
    _screen->drawRect(10,250,220,60, _box);
    _screen->setCursor(50,270);
    _screen->setTextSize(3);
    _screen->setTextColor(_text);
    _screen->print("Zurueck");
}