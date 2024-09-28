#include "rtc.h"

RTC_DS3231 _rtc;
extern DateTime date;

bool rtc_init() {
    if (!_rtc.begin()){
        Serial.println(F("RTC init failed"));
        return false;
    }
    _rtc.clearAlarm(1);
    _rtc.clearAlarm(2);
    Serial.println(F("RTC init OK"));
    return true;   
}

void rtc_set_time(DateTime d){
    if(_rtc.begin()){
        _rtc.adjust(d);
    }
}

uint8_t me_rtc() {
    static state_rtc_t state_rtc = RTC_INIT;

    switch(state_rtc) {
        case RTC_INIT:
            if(rtc_init()) {
                Serial.println("rtc_init() OK!");
                state_rtc = RTC_READ;
            } else {
                // como vamos a manejar los errores?
                return ME_ERROR;
            }
            break;  
        case RTC_READ:
            date = _rtc.now();
            Serial.printf("%02d/%02d/%d - %02d:%02d:%02d\n", date.day(), date.month(), date.year(), date.hour(), date.minute(), date.second());
            state_rtc = RTC_READ;
            return ME_FINISHED;
            break;
    }
    return ME_PROCESSING;
}