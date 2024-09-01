#include "ezo_ph.h"
#include "shift_reg.h"

void setup() {
    Serial.begin(9600); //Set the hardware serial port to 9600
    init_pH_probe();
    init_shift_reg();
    Serial.println();    
}

void loop() {
    me_ph();
    shift_reg_handle();
}

