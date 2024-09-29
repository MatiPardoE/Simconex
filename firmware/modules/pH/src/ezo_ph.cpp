#include "ezo_ph.h"
#include <ArduinoLog.h>

extern pH pH_Device;

Sequencer2 read_seq(&read_step1, READING_DELAY, &read_step2, 0);

Sequencer2 calib_clear_seq(&calib_clear_step1, CLEAR_DELAY, &calib_clear_step2, 0);
Sequencer2 calib_mid_seq(&calib_mid_step1, READING_DELAY, &calib_mid_step2, 0);
Sequencer2 calib_low_seq(&calib_low_step1, READING_DELAY, &calib_low_step2, 0);
Sequencer2 calib_high_seq(&calib_high_step1, READING_DELAY, &calib_high_step2, 0);
Sequencer2 calib_check_seq(&calib_check_step1, CLEAR_DELAY, &calib_check_step2, 0);

bool init_pH_probe()
{

    delay(3000);                     // wait for devices to boot
    Wire.beginTransmission(PH_ADDR); // try to talk to the device over I2C
    if (Wire.endTransmission() == 0)
    {                                   // check if communication attempt was successful
        pH_Device.set_address(PH_ADDR); // set that to be the address
        pH_Device.send_cmd("I");        // ask what kind of device it is
        delay(300);
        receive_and_print_response(pH_Device); // print the response
        read_seq.reset();                      // initialize the sequencer
        return true;
    }
    return false;
}

void read_step1()
{
    // send a read command
    Serial.println("read_step1");
    pH_Device.send_read_cmd();
}

void read_step2()
{
    // get the reading from the device
    Serial.println("read_step2");
    pH_Device.receive_read_cmd();
}

void calib_clear_step1()
{
    // send a read command
    Serial.println("calib_clear_step1");
    pH_Device.send_cmd("cal,clear");
}

void calib_clear_step2()
{
    // get the reading from the device
    Serial.println("calib_clear_step2");
    receive_and_print_response(pH_Device);
    Serial.println("");
}

void calib_mid_step1()
{
    // send a read command
    Serial.println("calib_mid_step1");
    pH_Device.send_cmd("cal,mid,7.00");
}

void calib_mid_step2()
{
    // get the reading from the device
    Serial.println("calib_mid_step2");
    receive_and_print_response(pH_Device);
    Serial.println("");
}

void calib_low_step1()
{
    // send a read command
    Serial.println("calib_low_step1");
    pH_Device.send_cmd("cal,low,4.00");
}

void calib_low_step2()
{
    // get the reading from the device
    Serial.println("calib_low_step2");
    receive_and_print_response(pH_Device);
    Serial.println("");
}

void calib_high_step1()
{
    // send a read command
    Serial.println("calib_high_step1");
    pH_Device.send_cmd("cal,high,10.00");
}

void calib_high_step2()
{
    // get the reading from the device
    Serial.println("calib_high_step2");
    receive_and_print_response(pH_Device);
    Serial.println("");
}

void calib_check_step1()
{
    // send a read command
    Serial.println("calib_check_step1");
    pH_Device.send_read_cmd();
}

void calib_check_step2()
{
    // get the reading from the device
    Serial.println("calib_check_step2");
    receive_and_print_response(pH_Device);
    Serial.println("");
}

void print_help()
{
    Serial.println(F("Atlas Scientific I2C Scan Demo                                             "));
    Serial.println(F("Commands:                                                                  "));
    Serial.println(F("poll         Takes readings continuously of all sensors                    "));
    Serial.println(F("addr,[nnn]   Changes the sensors I2C address to number nnn                 "));
}

uint8_t me_ph()
{
    static state_ph_t state_ph = READ_PH;

    switch (state_ph)
    {
    case READ_PH:
        if (read_seq.run() == read_seq.FINISHED)
        {
            if (pH_Device.get_error() == pH::SUCCESS)
            {
                Serial.printf("Valor de pH: %.2f \n", pH_Device.get_last_received_reading());
                return ME_FINISHED;
            }
            else
            {
                Serial.println("ph read error");
            }
            state_ph = CALIB_PH;
            delay(1000);
        }
        break;
    case CALIB_PH:
        while (1)
            ; // lo cuelgo
        if (me_calib_ph() == ME_FINISHED)
        {
            state_ph = READ_PH;
            delay(1000);
        }
        break;

    default: // error
        state_ph = READ_PH;
        break;
    }
    return ME_PROCESSING;
}

uint8_t me_calib_ph()
{
    static state_calib_t state_calib = INIT_CALIB;
    static state_calib_t previous_state_calib = NONE;
    static unsigned long last_read_time = 0;

    switch (state_calib)
    {
    case INIT_CALIB:
        Log.info("Comienzo de proceso de calibracion de pH, presione enter para eliminar la calibracion actual...");
        previous_state_calib = INIT_CALIB;
        state_calib = WAIT_ENTER;
        break;
    case WAIT_ENTER:
        // Muestro cada 10 segundos la lectura, solamente en estados de medicion de ph
        if (millis() - last_read_time >= 10000 &&
            (previous_state_calib == CLEAR_CALIB || previous_state_calib == LOW_POINT || previous_state_calib == LOW_POINT))
        {
            if (read_seq.run() == read_seq.FINISHED)
            {
                last_read_time = millis();

                if (pH_Device.get_error() == pH::SUCCESS)
                {
                    Log.info("Valor de pH: %.2f", pH_Device.get_last_received_reading());
                }
                else
                {
                    Log.error("ph read error in Calib state");
                }
            }
        }

        if (Serial.available() > 0)
        {
            char c = Serial.read();
            if (c == '\n')
            {
                // Voy al siguiente estado
                state_calib = static_cast<state_calib_t>(previous_state_calib + 1); // OJO que va en el orden del ENUM
            }
        }
        break;
    case CLEAR_CALIB:
        if (calib_clear_seq.run() == calib_check_seq.FINISHED)
        {
            previous_state_calib = CLEAR_CALIB;
            state_calib = WAIT_ENTER;

            Log.info("Calibracion reseteada");
            // Mensaje instrucciones para Mid Point
            Log.info("Coloque la sonda en la solucion de calibracion de 7.00");
            Log.info("Espere que se estabilice la medicion y presione enter para continuar...");
        }
        break;
    case MID_POINT:
        if (calib_mid_seq.run() == calib_mid_seq.FINISHED)
        {
            previous_state_calib = MID_POINT;
            state_calib = WAIT_ENTER;
            Log.info("Calibracion en ph 7.00 finalizada");
            // Mensaje instrucciones para Low Point
            Log.info("Coloque la sonda en la solucion de calibracion de 4.00");
            Log.info("Espere que se estabilice la medicion y presione enter para continuar...");
        }
        break;

    case LOW_POINT:
        if (calib_low_seq.run() == calib_low_seq.FINISHED)
        {
            previous_state_calib = LOW_POINT;
            state_calib = WAIT_ENTER;
            Log.info("Calibracion en ph 4.00 finalizada");
            // Mensaje instrucciones para High Point
            Log.info("Coloque la sonda en la solucion de calibracion de 10.00");
            Log.info("Espere que se estabilice la medicion y presione enter para continuar...");
        }
        break;

    case HIGH_POINT:
        if (calib_high_seq.run() == calib_high_seq.FINISHED)
        {
            previous_state_calib = HIGH_POINT;
            state_calib = CHECK_CALIB; // Aca ya no espero al enter
            Log.info("Calibracion en ph 10.00 finalizada");
            Log.info("Vamos a chequear la calibracion");
        }
        break;

    case CHECK_CALIB:
        if (calib_check_seq.run() == calib_check_seq.FINISHED)
        {
            state_calib = INIT_CALIB;
            previous_state_calib = NONE;
            return ME_FINISHED;
        }
        break;

    default: // error
        state_calib = MID_POINT;
        break;
    }

    return ME_PROCESSING;
}