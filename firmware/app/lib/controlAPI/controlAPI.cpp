#include "controlAPI.h"
#include <Arduino.h>
#include "rdoApiCode.h"

ControlAPI::ControlAPI()
{
    measuresAndOutputs = {0, 0, 0, 0, 0, 0, 0, 0, false, false, false, false};
    goalValues = {0, 0, 0, 0, 0, 0, 0, 0};
}

bool ControlAPI::run(cycle_manager::CycleStatus cycleStatus)
{
    static cycle_manager::MeasuresAndOutputs new_measure_calib = {0, 0, 0, 0, 0, 0, 0, 0, false, false, false, false};

    float ph_response = 0;
    // Medicion de ph
    if (can_ph_read())
    {
        ph_response = get_ph();
        if (ph_response != -1)
        {
            // ESP_LOGI("PH", "Valor de pH: %.2f", ph_response);
            newMeasureFlag.ph = true;
        }
    }

    if(newMeasureFlag.ph){
        ph_filter_total_sum -= ph_filter[0];
        for (int i = 0; i < PH_FILTER_SIZE - 1; i++) {
            ph_filter[i] = ph_filter[i + 1];
        }
        ph_filter[PH_FILTER_SIZE - 1] = ph_response;
        ph_filter_total_sum += ph_response;
        measuresAndOutputs.ph = ph_filter_total_sum / PH_FILTER_SIZE;
    }

    // Medicion OD
    if (_TIMEOUT_TO_RDO_REQUEST_)
    {
        requestRDO(&rdo);
        _updateTimeout_;
    }
    if (measuresAndOutputs.oxygen != rdo.doSaturation.measuredValue)
    {
        // ESP_LOGI("O2", "Nueva Medicion de Oxigeno: %.2f , Previa: %.2f ", rdo.doSaturation.measuredValue, measuresAndOutputs.oxygen);
        measuresAndOutputs.oxygen = rdo.doSaturation.measuredValue;
        newMeasureFlag.oxygen = true;
    }
    if (measuresAndOutputs.temperature != rdo.temperature.measuredValue)
    {
        measuresAndOutputs.temperature = rdo.temperature.measuredValue;
        newMeasureFlag.temperature = true;
    }
    if (measuresAndOutputs.concentration != rdo.doConcentration.measuredValue)
    {
        measuresAndOutputs.concentration = rdo.doConcentration.measuredValue;
        newMeasureFlag.concentration = true;
    }

    OD_modulation_run();

    // para calibrar y ver valores
    if (rdo.onCalibration == true)
    {
        if (new_measure_calib.oxygen != rdo.doSaturation.measuredValue || new_measure_calib.temperature != rdo.temperature.measuredValue)
        {
            new_measure_calib.oxygen = rdo.doSaturation.measuredValue;
            new_measure_calib.temperature = rdo.temperature.measuredValue;
            new_measure_calib.concentration = rdo.doConcentration.measuredValue;
            cm.sendDataToUI(new_measure_calib, 0);
        }
    }

    switch (cycleStatus)
    {
    case cycle_manager::CycleStatus::CYCLE_RUNNING:
        // TODO pasar a funcion

        air_pump_control(shiftRegister.getOutputState());
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
        }
        else
        {
            shiftRegister.setOutput(CO2, LOW);
        }

        if (__OD_IS_WORKING__ && __NOT_FREE_OD__)
        {
            if (newMeasureFlag.oxygen)
            {
                ESP_LOGI("OD", "Nueva Medicion de Oxigeno: %.2f , Previa: %.2f ", measuresAndOutputs.oxygen);
                OD_modulation_control(rdo.doSaturation.measuredValue, goalValues.oxygen);
            }
        }
        else
        {
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

        do_control_temp(measuresAndOutputs.temperature);

        break;
    default:
        break;
    }

    newMeasureFlag.ph = false;
    newMeasureFlag.oxygen = false;
    newMeasureFlag.temperature = false;
    newMeasureFlag.concentration = false;
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
        shiftRegister.setOutput(AIR_PUMP_FIX, LOW);
        ESP_LOGI("Manual", "Set EV_air to 0");
    }
    else if (command.startsWith("#A1"))
    {
        shiftRegister.setOutput(AIR_PUMP_FIX, HIGH);
        ESP_LOGI("Manual", "Set EV_air to 1");
    }
    else if (command.startsWith("#WCOLD0"))
    {
        // de forma manual, hago que sean inversos
        shiftRegister.setOutput(W_COLD, LOW);
        ESP_LOGI("Manual", "Set WaterCold to 0");
    }
    else if (command.startsWith("#WCOLD1"))
    {
        // de forma manual, hago que sean inversos
        shiftRegister.setOutput(W_COLD, HIGH);
        ESP_LOGI("Manual", "Set WaterCold to 1");
        shiftRegister.setOutput(W_HOT, LOW);
        ESP_LOGI("Manual", "Set WaterHot to 0");
    }
    else if (command.startsWith("#WHOT0"))
    {
        // de forma manual, hago que sean inversos
        shiftRegister.setOutput(W_HOT, LOW);
        ESP_LOGI("Manual", "Set WaterHot to 0");
    }
    else if (command.startsWith("#WHOT1"))
    {
        // de forma manual, hago que sean inversos
        shiftRegister.setOutput(W_COLD, LOW);
        ESP_LOGI("Manual", "Set WaterCold to 0");
        shiftRegister.setOutput(W_HOT, HIGH);
        ESP_LOGI("Manual", "Set WaterHot to 1");
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
    shiftRegister.setOutput(W_HOT, LOW);
    shiftRegister.setOutput(W_COLD, LOW);
    ledStripT.setDuty(0);
    ledStripMT.setDuty(0);
    ledStripMM.setDuty(0);
    ledStripML.setDuty(0);
    ledStripL.setDuty(0);
    shiftRegister.setOutput(AIR_PUMP_FIX, HIGH);

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
    ledStripT.begin(PIN_LED_STRIP_1, 0, 5000, 8);  // Configura el pin 5, canal 0, frecuencia de 5000 Hz, resolución de 8 bits
    ledStripMT.begin(PIN_LED_STRIP_2, 1, 5000, 8); // Configura el pin 18, canal 1, frecuencia de 5000 Hz, resolución de 8 bits
    ledStripMM.begin(PIN_LED_STRIP_3, 2, 5000, 8); // Configura el pin 19, canal 2, frecuencia de 5000 Hz, resolución de 8 bits
    ledStripML.begin(PIN_LED_STRIP_4, 3, 5000, 8); // Configura el pin 21, canal 3, frecuencia de 5000 Hz, resolución de 8 bits
    ledStripL.begin(PIN_LED_STRIP_5, 4, 5000, 8);  // Configura el pin 22, canal 4, frecuencia de 5000 Hz, resolución de 8 bits

    shiftRegister.begin(SR_DATA_PIN, SR_LATCH_PIN, SR_CLOCK_PIN);
    shiftRegister.setOutput(W_COLD, LOW);
    shiftRegister.setOutput(W_HOT, LOW);
    shiftRegister.setOutput(O2, LOW);
    shiftRegister.setOutput(N2, LOW);
    shiftRegister.setOutput(CO2, LOW);
    shiftRegister.setOutput(EV_1, LOW);
    shiftRegister.setOutput(EV_2, LOW);

    pinMode(OLD_AIR_PUMP, INPUT_PULLDOWN);
    shiftRegister.setOutput(AIR_PUMP_FIX, HIGH);

    return true;
}

cycle_manager::MeasuresAndOutputs ControlAPI::takeMeasuresAndOutputs()
{
    byte output_shift = shiftRegister.getOutputState();
    measuresAndOutputs.EV_oxygen = (output_shift & 0x02) == 0x02;
    measuresAndOutputs.EV_nitrogen = (output_shift & 0x04) == 0x04;
    measuresAndOutputs.EV_co2 = (output_shift & 0x08) == 0x08;
    measuresAndOutputs.light_top = ledStripT.getDuty();
    measuresAndOutputs.light_mid_top = ledStripMT.getDuty();
    measuresAndOutputs.light_mid_mid = ledStripMM.getDuty();
    measuresAndOutputs.light_mid_low = ledStripML.getDuty();
    measuresAndOutputs.light_low = ledStripL.getDuty();
    measuresAndOutputs.temperature = rdo.temperature.measuredValue;
    measuresAndOutputs.oxygen = rdo.doSaturation.measuredValue;
    measuresAndOutputs.concentration = rdo.doConcentration.measuredValue;
    measuresAndOutputs.air_pump = (output_shift & 0x32) == 0x32;
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

bool ControlAPI::air_pump_control(byte output_shift)
{
    bool EV_oxygen = (output_shift & 0x02) == 0x02;
    bool EV_nitrogen = (output_shift & 0x04) == 0x04;
    bool EV_co2 = (output_shift & 0x08) == 0x08;

    if (!EV_oxygen && !EV_nitrogen && !EV_co2)
    {
        shiftRegister.setOutput(AIR_PUMP_FIX, HIGH);
    }
    else if (EV_co2)
    {
        shiftRegister.setOutput(AIR_PUMP_FIX, LOW);
    }
    else if (EV_nitrogen && goalValues.oxygen >= 100.0)
    {
        shiftRegister.setOutput(AIR_PUMP_FIX, HIGH);
    }
    else if (EV_oxygen && goalValues.oxygen < 100.0)
    {
        shiftRegister.setOutput(AIR_PUMP_FIX, HIGH);
    }
    else{
        shiftRegister.setOutput(AIR_PUMP_FIX, LOW);
    }
    return true;
}

bool ControlAPI::do_control_temp(float temp)
{
    // devuelve true si esta en equilibrio
    if (goalValues.temperature == 0)
    {
        return false;
    }
    if (__TempisLower__(temp))
    {
        shiftRegister.setOutput(W_COLD, LOW);
        shiftRegister.setOutput(W_HOT, HIGH);
        return false;
    }
    else if (__TempisHigher__(temp))
    {
        shiftRegister.setOutput(W_COLD, HIGH);
        shiftRegister.setOutput(W_HOT, LOW);
        return false;
    }
    else
    {
        shiftRegister.setOutput(W_COLD, LOW);
        shiftRegister.setOutput(W_HOT, LOW);
        return true;
    }
}

bool ControlAPI::OD_modulation_control(float current, float goal)
{
    float delta = 0;
    if (__O2_LOWER_SAT__ && !o2_modulation_on)
    {
        delta = goal - current;
        time_o2_on_ms = round((delta / O2_DELTA_1S) * 1000); // 2.2% es lo que sube el oxigeno por segundo, si aumento va a cortar antes
        timestamp_o2_on = millis();
        shiftRegister.setOutput(O2, HIGH);
        shiftRegister.setOutput(N2, LOW);
        o2_modulation_on = true;
        ESP_LOGI("O2", "O2 ON for %d ms", time_o2_on_ms);
        ESP_LOGI("O2", "Delta: %.2f", delta);
        ESP_LOGI("O2", "Current O2: %.2f", current);
        ESP_LOGI("O2", "Goal O2: %.2f", goal);
    }
    else if (__O2_HIGHER_SAT__)
    {
        shiftRegister.setOutput(O2, LOW);
        shiftRegister.setOutput(N2, HIGH);
    }
    else if (__O2_IN_RANGE__)
    {
        shiftRegister.setOutput(O2, LOW);
        shiftRegister.setOutput(N2, LOW);
    }
    return false;
}

bool ControlAPI::OD_modulation_run()
{
    if (o2_modulation_on)
    {
        if (millis() - timestamp_o2_on > time_o2_on_ms)
        {
            shiftRegister.setOutput(O2, LOW);
            o2_modulation_on = false;
            ESP_LOGI("O2", "O2 OFF");
            return true;
        }
    }
    return false;
}
