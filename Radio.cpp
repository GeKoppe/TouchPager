#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Arduino.h>

// Local includes
#include "Radio.h"

/************************* PUBLIC *****************************/
Radio::Radio(uint16_t ce, uint16_t csn) {
    Serial.println("We in Radio Constructor");
    _ce = ce;
    _csn = csn;
    _readingPipe = 1;

    _listening = true;
    _level = RF24_PA_MIN;

    _antenna = RF24(_ce, _csn);
    Serial.println("We out of Radio Constructor");
}

void Radio::init() {
    _antenna.begin();
    _antenna.openReadingPipe(_readingPipe, _adress);
    _antenna.setPALevel(_level);
    _antenna.startListening();
}

void Radio::setPALevel(String level) {
    if (level == "MIN") {
        _level = RF24_PA_MIN;
    } else if (level == "LOW") {
        _level = RF24_PA_LOW;
    } else if (level == "HIGH") {
        _level = RF24_PA_HIGH;
    } else if (level == "MAX") {
        _level = RF24_PA_MAX;
    } else {
        _level = RF24_PA_MIN;
    }
}

void Radio::switchState() {
    _listening = !_listening;

    if (_listening) {
        _antenna.openReadingPipe(_readingPipe, _adress);
        _antenna.startListening();
    } else {
        _antenna.stopListening();
        _antenna.openWritingPipe(_adress);
    }

    _antenna.setPALevel(_level);
}

bool Radio::available(void) {
    return _antenna.available();
}

/**
 * @brief 
 * Checks, whether there are devices available nearby
 * 
 * @return true If there are devices available nearby 
 * @return false otherwise
 */
bool Radio::checkNearbyDevices(void) {
    void *buffer = malloc(255*sizeof(char));
    String msg;
    if (_listening) {
        switchState();
    }

    for (int i = 0; i < 3; i++) {
        _antenna.write(&_acknowledge, sizeof(_acknowledge));
        delay(10);
    }

    switchState();

    int counter = 0;
    while (!_antenna.available()) {
        counter++;
        if (counter > 9) return false;
        delay(10);
        continue;
    }

    _antenna.read(buffer, 255*sizeof(char));
    msg = String((char*)buffer);

    if (msg == _acknowledge) return true;

    return false;
}

String Radio::receiveMessage(void) {
    void *buffer = malloc(255*sizeof(char));
    String msg;

    if (!_listening) switchState();

    // Universal break character
    if (!_antenna.available()) return "\0";

    _antenna.read(buffer, 255*sizeof(char));

    msg = String((char*)buffer);

    if (msg == String(_acknowledge)) {
        acknowledge();
        return "\0";
    }

    if (msg == String(_jam)) {
        return String(_jam);
    }

    return msg;
}

void Radio::acknowledge(void) {
    if (_listening) switchState();

    for (int i = 0; i < 3; i++) {
        _antenna.write(&_acknowledge, sizeof(_acknowledge));
        delay(10);
    }
}

bool Radio::sendMessage(String msg) {
    if (_listening) switchState();

    _antenna.write(&msg, msg.length());

    return true;
}