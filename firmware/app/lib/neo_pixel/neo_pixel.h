#ifndef NEO_PIXEL_H
#define NEO_PIXEL_H

#include <Arduino.h>
#include <esp_log.h>
#include <Adafruit_NeoPixel.h>

class NeoPixel
{
public:
    enum neoPixelState
    {
        OFF,
        MANUAL_MODE,
        CYCLE_RUNNING,
        CYCLE_PAUSED,
        CYCLE_COMPLETED,
        CYCLE_ERROR,
        CALIB_PH,
        CALIB_OD,
        NEW_INTERVAL
    };
    NeoPixel(int pin, int numPixels);
    ~NeoPixel();

    void begin();
    void setState(neoPixelState state);
    void initFun();

private:
    Adafruit_NeoPixel pixels;
    int pin;
    int numPixels;
};

#endif // NEO_PIXEL_H