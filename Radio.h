#ifndef radio_h
#define radio_h

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Arduino.h>

class Radio {
    public:
        Radio(uint16_t ce, uint16_t csn);

        uint16_t    getCE (void) { return _ce; };
        uint16_t    getCSN (void) { return _csn; };

        uint8_t     getPALevel(void) { return _level; };
        void        setPALevel(String level);

        int         getReadingPipe (void) { return _readingPipe; };

        byte*       getAdress (void) { return _adress; };

        void        setCE (uint16_t ce) { _ce = ce; };
        void        setCSN (uint16_t csn) { _csn = csn; };
        void        setListening (bool listen) { _listening = listen; };

        bool        getListening (void) { return _listening; };

        void        switchState(void);

        String      receiveMessage(void);
        bool        available(void);
        void        init();

        bool        sendMessage(String msg);

    private:
        uint16_t _ce, _csn, _readingPipe, _paLevel;
        
        uint8_t _level = RF24_PA_LOW;

        bool _listening = true;

        byte _adress[6] = {
            0,0,0,0,1,'\0'
        };

        String _jam = "1337";
        String _acknowledge = "4269";

        RF24 _antenna;

        bool checkNearbyDevices(void);
        void acknowledge(void);

        void convertStringToCharArray(String s, char a[256]);
        String convertCharArrayToString(char a[256]);
};

#endif