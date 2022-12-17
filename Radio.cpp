#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Local includes
#include "Radio.h"

/************************* PUBLIC *****************************/
Radio::Radio(uint16_t ce, uint16_t csn, byte adress[6]) {
    _ce = ce;
    _csn = csn;
    _readingPipe = 1;
    _listening = true;
    _level = RF24_PA_MIN;

    for (int i = 0; i < 6; i++) {
        _adress[i] = adress[i];
    }

    _antenna = RF24(_ce, _csn);
    
    _antenna.begin();
    _antenna.openReadingPipe(_readingPipe, _adress);
    _antenna.setPALevel(_level);
    _antenna.startListening();
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
        _antenna.write(_acknowledge, sizeof(_acknowledge));
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

    if (msg == String(_acknowledge)) return true;

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

    if (msg == _acknowledge) {
        acknowledge();
        return "\0";
    }

    if (msg == _jam) {
        return String(_jam);
    }

    return msg;
}

void Radio::acknowledge(void) {
    if (_listening) switchState();

    for (int i = 0; i < 3; i++) {
        _antenna.write(_acknowledge, sizeof(_acknowledge));
        delay(10);
    }
}