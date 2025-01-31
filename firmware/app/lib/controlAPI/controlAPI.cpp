#include "controlAPI.h"
#include <Arduino.h>
#include "rdoApiCode.h"

ControlAPI::ControlAPI()
{
    measuresAndOutputs = {7.2, 0, 0, 0, false, false, false, false}; // ph = 7.2 to force change in led, oxygen, temperature, light, EV-1, EV-2, EV-3, EV-4
    goalValues = {0, 0, 0, 0};
}

bool ControlAPI::run(cycle_manager::CycleStatus cycleStatus)
{
    // Medicion de ph
    float ph_response = get_ph();
    if (ph_response != -1)
    {
        measuresAndOutputs.ph = ph_response;
        // Serial.printf("pH: %02.2f \n", ph_response);
    }

    // Medicion OD
    if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
        //ESP_LOGI("RDO", "rDO Status: %d",rdo.status);
        requestRDO( &rdo );
        _updateTimeout_;
    }

    switch (cycleStatus)
    {
    case cycle_manager::CycleStatus::CYCLE_RUNNING:
        // TODO pasar a funcion
        // Umbrales de control
        if (measuresAndOutputs.ph < goalValues.ph + 0.1)
        {
            shiftRegister.setOutput(CO2, HIGH);
            shiftRegister.setOutput(N2, LOW);
            shiftRegister.setOutput(AIR, LOW);
            shiftRegister.setOutput(HEATER, LOW);
            shiftRegister.setOutput(COOLER, LOW);
            shiftRegister.setOutput(EV_1, LOW);
            shiftRegister.setOutput(EV_2, LOW);
        }
        else if (measuresAndOutputs.ph > goalValues.ph - 0.1)
        {
            shiftRegister.setOutput(CO2, LOW);
            shiftRegister.setOutput(N2, HIGH);
            shiftRegister.setOutput(AIR, LOW);
            shiftRegister.setOutput(HEATER, LOW);
            shiftRegister.setOutput(COOLER, LOW);
            shiftRegister.setOutput(EV_1, LOW);
            shiftRegister.setOutput(EV_2, LOW);
        }

        if (ledStrip1.getDuty() != goalValues.light)
        {
            ledStrip1.setDuty(goalValues.light);
            ledStrip2.setDuty(goalValues.light);
            ledStrip3.setDuty(goalValues.light);
            ledStrip4.setDuty(goalValues.light);
            ledStrip5.setDuty(goalValues.light);
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
        ledStrip1.setDuty(value);
        ledStrip2.setDuty(value);
        ledStrip3.setDuty(value);
        ledStrip4.setDuty(value);
        ledStrip5.setDuty(value);
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
    init_pH_probe();
    ledStrip1.begin(PIN_LED_STRIP_1, 0, 5000, 8); // Configura el pin 5, canal 0, frecuencia de 5000 Hz, resolución de 8 bits
    ledStrip2.begin(PIN_LED_STRIP_2, 1, 5000, 8); // Configura el pin 18, canal 1, frecuencia de 5000 Hz, resolución de 8 bits
    ledStrip3.begin(PIN_LED_STRIP_3, 2, 5000, 8); // Configura el pin 19, canal 2, frecuencia de 5000 Hz, resolución de 8 bits
    ledStrip4.begin(PIN_LED_STRIP_4, 3, 5000, 8); // Configura el pin 21, canal 3, frecuencia de 5000 Hz, resolución de 8 bits
    ledStrip5.begin(PIN_LED_STRIP_5, 4, 5000, 8); // Configura el pin 22, canal 4, frecuencia de 5000 Hz, resolución de 8 bits

    for (int i = 100; i >= 0; i--)
    {
        ledStrip1.setDuty(i);
        ledStrip2.setDuty(i);
        ledStrip3.setDuty(i);
        ledStrip4.setDuty(i);
        ledStrip5.setDuty(i);
        delay(10);
    }

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
    measuresAndOutputs.EV_co2 = (output_shift & 0x01) == 0x01;
    measuresAndOutputs.EV_oxygen = (output_shift & 0x02) == 0x02;
    measuresAndOutputs.EV_nitrogen = (output_shift & 0x04) == 0x04;
    measuresAndOutputs.EV_air = (output_shift & 0x08) == 0x08;
    measuresAndOutputs.light = ledStrip1.getDuty();
    measuresAndOutputs.temperature = rdo.temperature.measuredValue;
    measuresAndOutputs.oxygen = rdo.doSaturation.measuredValue;
    // la medicion de ph se actualiza en el .run()

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