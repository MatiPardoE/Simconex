#include "LedStrip.h"

LedStrip::LedStrip() : _pin(-1), _channel(-1), _resolution(8), _maxDuty(255), _currentDuty(0) {
    // Log.info("LedStrip instance created.\n");
}

void LedStrip::begin(int pin, int channel, int freq, int resolution) {
    _pin = pin;
    _channel = channel;
    _resolution = resolution;
    _maxDuty = (1 << resolution) - 1;
    _currentDuty = 0;

    ledcSetup(_channel, freq, _resolution);
    ledcAttachPin(_pin, _channel);
    off();  // Apagar por defecto

    // Log.info("LedStrip initialized on pin %d, channel %d, freq %d, resolution %d\n", _pin, _channel, freq, _resolution);
}

void LedStrip::on() {
    ledcWrite(_channel, _currentDuty);
    // Log.info("LedStrip turned on with duty %d\n", _currentDuty);
}

void LedStrip::off() {
    ledcWrite(_channel, 0);
    // Log.info("LedStrip turned off\n");
}

void LedStrip::setDuty(int duty) {
    if (duty < 0){
        duty = 0;
        // Log.error("Duty cannot be negative.\n");
    }
    if (duty > 100){
        duty = 100;
        // Log.error("Duty cannot be greater than 100.\n");
    }
    _currentDuty = map(duty, 0, 100, 0, _maxDuty);
    ledcWrite(_channel, _currentDuty);
    // Log.info("Duty set to %d%% (PWM value: %d)\n", duty, _currentDuty);
}
