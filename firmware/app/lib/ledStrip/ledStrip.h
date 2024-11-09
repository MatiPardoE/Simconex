#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <Arduino.h>
#include <ArduinoLog.h>

class LedStrip {
public:
    LedStrip();
    void begin(int pin, int channel, int freq, int resolution);
    void on();
    void off();
    void setDuty(int duty); // duty en rango de 0 a 100
    int getDuty();
private:
    int _pin;
    int _channel;
    int _resolution;
    int _maxDuty;
    int _currentDuty;
};

#endif
