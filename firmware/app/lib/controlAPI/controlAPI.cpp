#include "controlAPI.h"
#include <Arduino.h>
#include "rdoApiCode.h"

ControlAPI::ControlAPI()
{
    measuresAndOutputs = {0, 0, 0, 0, false, false, false, false};
    goalValues = {0, 0, 0, 0};
}

bool ControlAPI::run(cycle_manager::CycleStatus cycleStatus)
{
    static cycle_manager::MeasuresAndOutputs new_measure_calib = {0, 0, 0, 0, false, false, false, false};

    float ph_response = 0;
    // Medicion de ph
    if (can_ph_read())
    {
        ph_response = get_ph();
        if (ph_response != -1)
        {
            //ESP_LOGI("PH", "Valor de pH: %.2f", ph_response);
            measuresAndOutputs.ph = ph_response;
        }
    }

    // Medicion OD
    if (_TIMEOUT_TO_RDO_REQUEST_)
    {
        requestRDO(&rdo);
        _updateTimeout_;
    }

    //para calibrar y ver valores
    if( rdo.onCalibration == true ){
        if(new_measure_calib.oxygen != rdo.doSaturation.measuredValue || new_measure_calib.temperature != rdo.temperature.measuredValue){
            new_measure_calib.oxygen = rdo.doSaturation.measuredValue;
            new_measure_calib.temperature = rdo.temperature.measuredValue;
            cm.sendDataToUI(new_measure_calib, 0);
        }
    }



    switch (cycleStatus)
    {
    case cycle_manager::CycleStatus::CYCLE_RUNNING:
        // TODO pasar a funcion
        // Umbrales de control
        if (__PH_IS_WORKING__ && __NOT_FREE_PH__)
        {
            if (__PH_LOWER__)
            {
                shiftRegister.setOutput(CO2, LOW);
            }
            else if (__PH_HIGHER__)
            {
                shiftRegister.setOutput(CO2, HIGH);
            }
        }else{
            shiftRegister.setOutput(CO2, LOW);
        }

        if(__OD_IS_WORKING__ && __NOT_FREE_OD__){
            if (__O2_LOWER_SAT__)
            {
                shiftRegister.setOutput(O2, HIGH);
                shiftRegister.setOutput(N2, LOW);
            }
            else if (__O2_HIGHER_SAT__)
            {
                shiftRegister.setOutput(O2, LOW);
                shiftRegister.setOutput(N2, HIGH);
            }
        }else{
            shiftRegister.setOutput(O2, LOW);
            shiftRegister.setOutput(N2, LOW);
        }

        if (ledStripT.getDuty() != goalValues.light_top)
        {
            ledStripT.setDuty(goalValues.light_top);
        }
        if (ledStripMT.getDuty() != goalValues.light_mid_top)
        {
            ledStripMT.setDuty(goalValues.light_mid_top);
        }
        if (ledStripMM.getDuty() != goalValues.light_mid_mid)
        {
            ledStripMM.setDuty(goalValues.light_mid_mid);
        }
        if (ledStripML.getDuty() != goalValues.light_mid_low)
        {
            ledStripML.setDuty(goalValues.light_mid_low);
        }
        if (ledStripL.getDuty() != goalValues.light_low)
        {
            ledStripL.setDuty(goalValues.light_low);
        }
        break;
    default:
        break;
    }

    return true;
}

bool ControlAPI::modeManualsetOutputs(String command)
{
    if (command.startsWith("#C0"))
    {
        shiftRegister.setOutput(CO2, LOW);
        ESP_LOGI("Manual", "Set EV_co2 to 0");
    }
    else if (command.startsWith("#C1"))
    {
        shiftRegister.setOutput(CO2, HIGH);
        ESP_LOGI("Manual", "Set EV_co2 to 1");
    }
    else if (command.startsWith("#O0"))
    {
        shiftRegister.setOutput(O2, LOW);
        ESP_LOGI("Manual", "Set EV_o2 to 0");
    }
    else if (command.startsWith("#O1"))
    {
        shiftRegister.setOutput(O2, HIGH);
        ESP_LOGI("Manual", "Set EV_o2 to 1");
    }
    else if (command.startsWith("#N0"))
    {
        shiftRegister.setOutput(N2, LOW);
        ESP_LOGI("Manual", "Set EV_n2 to 0");
    }
    else if (command.startsWith("#N1"))
    {
        shiftRegister.setOutput(N2, HIGH);
        ESP_LOGI("Manual", "Set EV_n2 to 1");
    }
    else if (command.startsWith("#A0"))
    {
        shiftRegister.setOutput(AIR, LOW);
        ESP_LOGI("Manual", "Set EV_air to 0");
    }
    else if (command.startsWith("#A1"))
    {
        shiftRegister.setOutput(AIR, HIGH);
        ESP_LOGI("Manual", "Set EV_air to 1");
    }
    else if (command.startsWith("#COLD0"))
    {
        shiftRegister.setOutput(COOLER, LOW);
        ESP_LOGI("Manual", "Set CoolerBomb to 0");
    }
    else if (command.startsWith("#COLD1"))
    {
        shiftRegister.setOutput(COOLER, HIGH);
        ESP_LOGI("Manual", "Set CoolerBomb to 1");
    }
    else if (command.startsWith("#HOT0"))
    {
        shiftRegister.setOutput(HEATER, LOW);
        ESP_LOGI("Manual", "Set HeaterBomb to 0");
    }
    else if (command.startsWith("#HOT1"))
    {
        shiftRegister.setOutput(HEATER, HIGH);
        ESP_LOGI("Manual", "Set HeaterBomb to 1");
    }
    else if (command.startsWith("#L"))
    {
        int value = command.substring(2).toInt();
        ledStripT.setDuty(value);
        ledStripMT.setDuty(value);
        ledStripMM.setDuty(value);
        ledStripML.setDuty(value);
        ledStripL.setDuty(value);
        ESP_LOGI("Manual", "Set light to: %d", value);
    }
    return true;
}

bool ControlAPI::turnOffOutputs()
{
    shiftRegister.setOutput(0, LOW);
    shiftRegister.setOutput(1, LOW);
    shiftRegister.setOutput(2, LOW);
    shiftRegister.setOutput(3, LOW);
    return true;
}

bool ControlAPI::init()
{
    Wire.begin(I2C_SDA, I2C_SCL); // start the I2C
    if (init_pH_probe())
    {
        ESP_LOGI("PH", "pH probe initialized");
    }
    else
    {
        ESP_LOGE("PH", "pH probe not initialized");
    }
    ledStripT.begin(PIN_LED_STRIP_1, 0, 5000, 8); // Configura el pin 5, canal 0, frecuencia de 5000 Hz, resolución de 8 bits
    ledStripMT.begin(PIN_LED_STRIP_2, 1, 5000, 8); // Configura el pin 18, canal 1, frecuencia de 5000 Hz, resolución de 8 bits
    ledStripMM.begin(PIN_LED_STRIP_3, 2, 5000, 8); // Configura el pin 19, canal 2, frecuencia de 5000 Hz, resolución de 8 bits
    ledStripML.begin(PIN_LED_STRIP_4, 3, 5000, 8); // Configura el pin 21, canal 3, frecuencia de 5000 Hz, resolución de 8 bits
    ledStripL.begin(PIN_LED_STRIP_5, 4, 5000, 8); // Configura el pin 22, canal 4, frecuencia de 5000 Hz, resolución de 8 bits

    shiftRegister.begin(SR_DATA_PIN, SR_LATCH_PIN, SR_CLOCK_PIN);
    shiftRegister.setOutput(0, LOW);
    shiftRegister.setOutput(1, LOW);
    shiftRegister.setOutput(2, LOW);
    shiftRegister.setOutput(3, LOW);
    shiftRegister.setOutput(4, LOW);
    shiftRegister.setOutput(5, LOW);
    shiftRegister.setOutput(6, LOW);
    shiftRegister.setOutput(7, LOW);

    return true;
}

cycle_manager::MeasuresAndOutputs ControlAPI::takeMeasuresAndOutputs()
{
    byte output_shift = shiftRegister.getOutputState();
    measuresAndOutputs.EV_air = (output_shift & 0x01) == 0x01;
    measuresAndOutputs.EV_oxygen = (output_shift & 0x02) == 0x02;
    measuresAndOutputs.EV_nitrogen = (output_shift & 0x04) == 0x04;
    measuresAndOutputs.EV_co2 = (output_shift & 0x08) == 0x08;
    measuresAndOutputs.light = ledStripT.getDuty();
    measuresAndOutputs.temperature = rdo.temperature.measuredValue;
    measuresAndOutputs.oxygen = rdo.doSaturation.measuredValue;
    // la medicion de ph se actualiza en el .run()

    return measuresAndOutputs;
}

bool ControlAPI::set_control_var(cycle_manager::IntervalData intervalData)
{
    // TODO: Impkemnet max and min for each variable.
    goalValues.light_top = intervalData.light_top;
    goalValues.light_mid_top = intervalData.light_mid_top;
    goalValues.light_mid_mid = intervalData.light_mid_mid;
    goalValues.light_mid_low = intervalData.light_mid_low;
    goalValues.light_low = intervalData.light_low;
    goalValues.oxygen = intervalData.oxygen;
    goalValues.ph = intervalData.ph;
    goalValues.temperature = intervalData.temperature;

    return true;
}