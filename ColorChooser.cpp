#include "ColorChooser.h"

ColorChooser::ColorChooser(Elegoo_TFTLCD *screen, TouchScreen *ts, uint16_t bg, uint16_t box, uint16_t text) {
    _screen = screen;
    _ts = ts;
    _bg = bg;
    _box = box;
    _text = text;
}

int ColorChooser::choose() {
    uint16_t selection = -1;
    draw();
    while (true) {
        digitalWrite(13, HIGH);
        TSPoint p = _ts->getPoint();
        digitalWrite(13, LOW);

        if (p.z > 20) {
            ScreenParse parse;
            parse.x = (int) ((p.x - 120) / 3.5);
            parse.y = (int) ((p.y - 70) / 2.66);

            selection = getSelection(parse);

            if (selection != -1) break;
        }

        delay(10);
    }
    return selection;
}

uint16_t ColorChooser::getSelection(ScreenParse coords) {
    int column, row;

    Serial.println("X: " + String(coords.x) + ", Y: " + String(coords.y));
    if (320 - coords.y > 250) return 0;

    if (coords.x < 35 || coords.x > 215) return -1;
    if (coords.x < 135 && coords.x > 100) return -1;

    column = coords.x < 100 ? 0 : 1;
    row = (int)((320 - coords.y) / 80);

    return _colors[column + (row*2)];
}

void ColorChooser::draw() {
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    _screen->fillScreen(_bg);

    // Box Size: 70. Boxes start at y = 10 and end at y = 240, box offset 10
    for (int i = 0; i < 6; i++) {
        _screen->fillRect((i % 2 == 0 ? 35 : 135),
            ((int) (i / 2))*80 + 10,
            70,
            70,
            _colors[i]
        );
    }

    _screen->drawRect(10,250,220,60, _box);
    _screen->setCursor(50,270);
    _screen->setTextSize(3);
    _screen->setTextColor(_text);
    _screen->print("Zurueck");
}