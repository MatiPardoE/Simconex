#include <Wire.h>
#include <RTClib.h>
#include "utilities.h"

#define I2C_SDA 12
#define I2C_SCL 14

enum state_rtc_t {
    RTC_INIT,
    RTC_READ
};

bool rtc_init();
void rtc_set_time(DateTime d);
uint8_t me_rtc();