#include "sensorControl.h"

SensorControl::SensorControl()
{
    measuresAndOutputs = {0, 0, 0, 0, false, false, false, false};
    goalValues = {0, 0, 0, 0};
}

SensorControl::MeasuresAndOutputs SensorControl::takeMeasuresAndOutputs()
{
    return measuresAndOutputs;
}

bool SensorControl::set_control_var(cycle_manager::IntervalData intervalData)
{
    // TODO: Impkemnet max and min for each variable.
    goalValues.light = intervalData.light;
    goalValues.oxygen = intervalData.oxygen;
    goalValues.ph = intervalData.ph;
    goalValues.temperature = intervalData.temperature;
    
    return true;
}