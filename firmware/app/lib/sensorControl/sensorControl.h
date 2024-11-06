#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <string>
#include <cycle_manager.h>

class SensorControl
{
public:
    // Constructor
    SensorControl();

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
};

#endif // SENSOR_CONTROL_H