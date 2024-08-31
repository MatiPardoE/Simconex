#include <Arduino.h>
#include "spi_sd.h"
#include "rtc.h"

enum state_general_t {
    RTC_STATE,
    SD_STATE,
    FREEZE_STATE
};

void me_general();

void init_i2c() {
    Wire.begin(I2C_SDA, I2C_SCL);
}

void setup(){
    Serial.begin(9600);
    init_i2c();
}

void loop(){
    me_general();
}

void me_general(){
    static state_general_t state_general = SD_STATE;

    switch(state_general) {
        case RTC_STATE:
            if(!me_rtc()) {
                state_general = FREEZE_STATE;
            }
            break;

        case SD_STATE:
            if(!me_sd()) {
                state_general = FREEZE_STATE;
            }
            break;     

        case FREEZE_STATE:
            while(1);
            break;
    }    
}
