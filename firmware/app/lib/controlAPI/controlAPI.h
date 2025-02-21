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
#define O2 1
#define N2 2
#define CO2 3
#define W_COLD  0
#define W_HOT   6
#define EV_1 5
#define EV_2 4
////// Bomba de Aire ///////
#define OLD_AIR_PUMP 23
#define AIR_PUMP_FIX EV_1

#define __UMBRAL_PH__ 0.1
#define __PH_LOWER__ (measuresAndOutputs.ph < goalValues.ph - __UMBRAL_PH__)
#define __PH_HIGHER__ (measuresAndOutputs.ph > goalValues.ph + __UMBRAL_PH__)

#define __PH_IS_WORKING__ (measuresAndOutputs.ph != 0)
#define __OD_IS_WORKING__ (rdo.doSaturation.measuredValue != 0)

#define __NOT_FREE_PH__ (goalValues.ph != 0)
#define __NOT_FREE_OD__ (goalValues.oxygen != 0)

#define __UMBRAL_O2__ 3
#define __O2_LOWER_SAT__ (measuresAndOutputs.oxygen < goalValues.oxygen - __UMBRAL_O2__)
#define __O2_HIGHER_SAT__ (measuresAndOutputs.oxygen > goalValues.oxygen + __UMBRAL_O2__)
#define __O2_IN_RANGE__ (measuresAndOutputs.oxygen > goalValues.oxygen - __UMBRAL_O2__ && measuresAndOutputs.oxygen < goalValues.oxygen + __UMBRAL_O2__)

#define __UMBRAL_TEMP__ 0.5
#define __TempisLower__(x)     (x < goalValues.temperature - __UMBRAL_TEMP__)
#define __TempisHigher__(x)    (x > goalValues.temperature + __UMBRAL_TEMP__)


#define O2_DELTA_1S (15) // 2.2% es lo que sube el oxigeno por segundo (pasado a milisegundo)

#define PH_FILTER_SIZE 5
extern cycle_manager cm;

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
    
    bool do_control_temp( float temp );

private:
    struct GoalValues
    {
        float ph;
        float oxygen;
        float temperature;
        int light_top;
        int light_mid_top;
        int light_mid_mid;
        int light_mid_low;
        int light_low;
    };
    struct NewMeasureFlags
    {
        bool ph;
        bool oxygen;
        bool temperature;
        bool light_top;
        bool light_mid_top;
        bool light_mid_mid;
        bool light_mid_low;
        bool light_low;
        bool concentration;
    };
    // Add private members if needed
    cycle_manager::MeasuresAndOutputs measuresAndOutputs;
    cycle_manager::MeasuresAndOutputs measuresAndOutputs_prev;
    cycle_manager::MeasuresAndOutputs ev_was_active;
    GoalValues goalValues;
    NewMeasureFlags newMeasureFlag;
    // SENSORS
    LedStrip ledStripT, ledStripMT, ledStripMM, ledStripML, ledStripL;
    ShiftRegister74HC595 shiftRegister;

    bool air_pump_control(byte output_shift);
    bool OD_modulation_control(float current, float goal);
    bool OD_modulation_run();
    bool o2_modulation_on = false;
    uint16_t time_o2_on_ms = 0;
    unsigned long timestamp_o2_on = 0;

    float ph_filter[PH_FILTER_SIZE] = {0};
    float ph_filter_total_sum = 0;
};

#endif // SENSOR_CONTROL_H