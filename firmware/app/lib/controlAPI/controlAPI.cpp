#include "controlAPI.h"

ControlAPI::ControlAPI()
{
    measuresAndOutputs = {0, 0, 0, 0, false, false, false, false};     //ph, oxygen, temperature, light, EV-1, EV-2, EV-3, EV-4
    goalValues = {0, 0, 0, 0};
}

cycle_manager::MeasuresAndOutputs ControlAPI::takeMeasuresAndOutputs()
{
    return measuresAndOutputs;
}

bool ControlAPI::set_control_var(cycle_manager::IntervalData intervalData)
{
    // TODO: Impkemnet max and min for each variable.
    goalValues.light = intervalData.light;
    goalValues.oxygen = intervalData.oxygen;
    goalValues.ph = intervalData.ph;
    goalValues.temperature = intervalData.temperature;
    
    return true;
}