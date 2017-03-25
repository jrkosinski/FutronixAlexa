
#include "IRremoteESP8266.h"
#include <Arduino.h>

#define DAIKIN_COOL B011
#define DAIKIN_HEAT B100
#define DAIKIN_FAN B110
#define DAIKIN_AUTO B000
#define DAIKIN_DRY B010

#define DAIKIN_POWERFUL B00000010
#define DAIKIN_SILENT   B00100000
#define DAIKIN_COMMAND_LENGTH 27

class IRDaikinESP
{
    public:
        IRDaikinESP(int pin);
        //: IRsend(pin){};

        void send();

        void begin();
        void on();
        void off();
        uint8_t getPower();

        void setAux(uint8_t aux);
        uint8_t getAux();

        void setTemp(uint8_t temp);
        uint8_t getTemp();

        void setFan(uint8_t fan);
        uint8_t getFan();

        uint8_t getMode();
        void setMode(uint8_t mode);

        void setSwingVertical(uint8_t swing);
        uint8_t getSwingVertical();
        void setSwingHorizontal(uint8_t swing);
        uint8_t getSwingHorizontal();

    private:
        // # of bytes per command
        unsigned char daikin[DAIKIN_COMMAND_LENGTH] = {
        0x11,0xDA,0x27,0xF0,0x00,0x00,0x00,0x20,
        //0    1    2   3    4    5     6   7
        0x11,0xDA,0x27,0x00,0x00,0x41,0x1E,0x00,
        //8    9   10   11   12    13   14   15
        0xB0,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0xE3 };
        //16  17    18  19   20    21   22  23   24   25   26

        void checksum();

        IRsend _irsend;
};
