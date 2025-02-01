#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <string>
#include <cycle_manager.h>
#include "ezo_ph.h"
#include <ledStrip.h>
#include <shiftRegister74HC595.h>
#include <optional>
#include "esp_log.h"

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
/////////// Shift Register //////
#define AIR 0
#define O2 1
#define N2 2
#define CO2 3
#define HEATER 4
#define COOLER 5
#define EV_1 6
#define EV_2 7

#define __UMBRAL_PH__ 0.1
#define __PH_LOWER__ (measuresAndOutputs.ph < goalValues.ph - __UMBRAL_PH__)
#define __PH_HIGHER__ (measuresAndOutputs.ph > goalValues.ph + __UMBRAL_PH__)

#define __PH_IS_WORKING__ (measuresAndOutputs.ph != 0)
#define __OD_IS_WORKING__ (measuresAndOutputs.oxygen != 0)

#define __UMBRAL_O2__ 2
#define __O2_LOWER_SAT__ (measuresAndOutputs.oxygen < goalValues.oxygen + __UMBRAL_O2__)
#define __O2_HIGHER_SAT__ (measuresAndOutputs.oxygen > goalValues.oxygen - __UMBRAL_O2__)

#define set_light_duty_all(x)        \
    ledStrip1.setDuty(x);      \
    ledStrip2.setDuty(x >> 1); \
    ledStrip3.setDuty(x >> 2); \
    ledStrip4.setDuty(x >> 3); \
    ledStrip5.setDuty(x >> 4);
class ControlAPI
{
public:
    // Constructor
    ControlAPI();
    bool run(cycle_manager::CycleStatus cycleStatus);
    bool modeManualsetOutputs(String command);
    bool init();

    // Function to take measures
    // TODO: Implement the function to take measures
    cycle_manager::MeasuresAndOutputs takeMeasuresAndOutputs();
    bool turnOffOutputs();
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