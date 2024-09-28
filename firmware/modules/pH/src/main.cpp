#include <Arduino.h>
#include "spi_sd.h"
#include "rtc.h"
#include "utilities.h"
#include "ezo_ph.h"
#include "shift_reg.h"

enum state_fbr_t {
    FBR_READ_RTC,
    FBR_UPDATE_LEDS,
    FBR_MEAS_PH,
    FBR_MEAS_OD,
    FBR_MEAS_TEMP,
    FBR_SET_OUTPUTS,
    FBR_SAVE_LOG,
    FBR_FREEZE
};

/* VARIABLES HARDCODEADAS */
float pH_setpoint = 7;

/* VARIABLES DE CONTROL DE ENTRADAS Y SALIDAS */
DateTime date = DateTime(F(__DATE__), F(__TIME__)); // por ahora uso la hora de compilacion
pH pH_Device = pH(20, "EZO pH probe");   
byte shift_outputs = 0;

void me_fbr_simconex() {
    static state_fbr_t state_fbr = FBR_READ_RTC;

    switch(state_fbr){
        case FBR_READ_RTC:
            if(me_rtc() == ME_FINISHED) {
                state_fbr = FBR_MEAS_PH;
            }
            break;

        case FBR_MEAS_PH:
            if(me_ph() == ME_FINISHED) {
                state_fbr = FBR_SET_OUTPUTS;
            }
            break;

        case FBR_SET_OUTPUTS:
            if(pH_Device.get_last_received_reading() > (pH_setpoint + pH_setpoint*HYSTERESIS)) {
                ev_carbono_on();
            } else if (pH_Device.get_last_received_reading() < (pH_setpoint - pH_setpoint*HYSTERESIS)) {
                ev_carbono_off();
            }
            shift_reg_handle();
            state_fbr = FBR_SAVE_LOG;
            break;

        case FBR_SAVE_LOG:
            if(me_sd() == ME_FINISHED) {
                state_fbr = FBR_FREEZE; // default para colgarse
            }
            break;

        case FBR_FREEZE:
        default:
            Serial.println("Me cuelgo");
            while(1);
            break;
    }
}

void setup() {
    Serial.begin(9600); //Set the hardware serial port to 9600
    Wire.begin(I2C_SDA, I2C_SCL); //start the I2C
    init_pH_probe();
    init_shift_reg();
    Serial.println();    
}

void loop() {
    me_fbr_simconex();
}

