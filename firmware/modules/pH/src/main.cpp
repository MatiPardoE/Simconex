#include "ezo_ph.h"


bool polling = true; //variable to determine whether or not were polling the circuits

void setup() {
    Serial.begin(9600); //Set the hardware serial port to 9600
    init_pH_probe();
    Serial.println();    
}

void loop() {
    me_ph();
}

