#ifndef radio
#define radio

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

class Radio {
    public:
        Radio(uint16_t ce, uint16_t csn, byte adress[6]);

        uint16_t    getCE (void) { return _ce; };
        uint16_t    getCSN (void) { return _csn; };

        int         getReadingPipe (void) { return _readingPipe; };

        byte*       getAdress (void) { return _adress; };

        void        setCE (uint16_t ce) { _ce = ce; };
        void        setCSN (uint16_t csn) { _csn = csn; };
                    

    private:
        uint16_t _ce, _csn, _readingPipe, _paLevel;

        byte _adress[6];

        char* _jam = "1337";
        char* _acknowledge = "4269";

        RF24 _radio;
};

#endif