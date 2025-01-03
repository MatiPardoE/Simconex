#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <string>
#include <cycle_manager.h>
#include "ezo_ph.h"
#include <ledStrip.h>
#include <shiftRegister74HC595.h>

/////////// PINES DE NUESTRO MICROCONTROLADOR ///////////
/////////// LED STRIPS //////
#define PIN_LED_STRIP_1 5
#define PIN_LED_STRIP_2 18
#define PIN_LED_STRIP_3 19
#define PIN_LED_STRIP_4 21
#define PIN_LED_STRIP_5 22
/////////// SHIFT REGISTER //////
#define SR_DATA_PIN 15
#define SR_LATCH_PIN 2
#define SR_CLOCK_PIN 0

class ControlAPI
{
public:
    // Constructor
    ControlAPI();
    bool run();
    bool init();

    // Function to take measures
    // TODO: Implement the function to take measures
    cycle_manager::MeasuresAndOutputs takeMeasuresAndOutputs();

    bool set_control_var(cycle_manager::IntervalData intervalData);

private:
    struct GoalValues
    {
        float ph;
        float oxygen;
        float temperature;
        int light;
    };
    // Add private members if needed
    cycle_manager::MeasuresAndOutputs measuresAndOutputs;
    GoalValues goalValues;

    // SENSORS
    LedStrip ledStrip1, ledStrip2, ledStrip3, ledStrip4, ledStrip5;
    ShiftRegister74HC595 shiftRegister;
};

#endif // SENSOR_CONTROL_H