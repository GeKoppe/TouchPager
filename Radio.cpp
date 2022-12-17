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
    
    for (int i = 0; i < 6; i++) {
        _adress[i] = adress[i];
    }

    _radio = RF24(_ce, _csn);
    
    _radio.begin();
    _radio.openReadingPipe(_readingPipe, _adress);
    _radio.setPALevel(RF24_PA_MIN);
    _radio.startListening();
}

