#include "ezo_ph.h"

extern pH pH_Device;

Sequencer2 read_seq(&read_step1, READING_DELAY, &read_step2, 0 );

Sequencer2 calib_clear_seq(&calib_clear_step1,  CLEAR_DELAY,    &calib_clear_step2, 0);
Sequencer2 calib_mid_seq  (&calib_mid_step1,    READING_DELAY,  &calib_mid_step2,   0);
Sequencer2 calib_low_seq  (&calib_low_step1,    READING_DELAY,  &calib_low_step2,   0);
Sequencer2 calib_high_seq (&calib_high_step1,   READING_DELAY,  &calib_high_step2,  0);
Sequencer2 calib_check_seq(&calib_check_step1,  CLEAR_DELAY,    &calib_check_step2, 0);

bool init_pH_probe(){
    Wire.begin(I2C_SDA, I2C_SCL); //start the I2C
    delay(3000); //wait for devices to boot
    Wire.beginTransmission(PH_ADDR); //try to talk to the device over I2C
    if(Wire.endTransmission() == 0) { //check if communication attempt was successful   
        pH_Device.set_address(PH_ADDR); //set that to be the address
        pH_Device.send_cmd("I"); //ask what kind of device it is
        delay(300);
        receive_and_print_response(pH_Device); //print the response
        read_seq.reset(); //initialize the sequencer
        return true;
    }
    return false;
}

void read_step1() {
    //send a read command
    Serial.println("read_step1");
    pH_Device.send_read_cmd();
}

void read_step2() {
    //get the reading from the device
    Serial.println("read_step2");
    pH_Device.receive_read_cmd();
}

void calib_clear_step1() {
    //send a read command
    Serial.println("calib_clear_step1");
    pH_Device.send_cmd("cal,clear");
}

void calib_clear_step2() {
    //get the reading from the device
    Serial.println("calib_clear_step2");
    receive_and_print_response(pH_Device);             
    Serial.println("");
}

void calib_mid_step1() {
    //send a read command
    Serial.println("calib_mid_step1");
    pH_Device.send_cmd("cal,mid,7.00");
}

void calib_mid_step2() {
    //get the reading from the device
    Serial.println("calib_mid_step2");
    receive_and_print_response(pH_Device);             
    Serial.println("");
}

void calib_low_step1() {
    //send a read command
    Serial.println("calib_low_step1");
    pH_Device.send_cmd("cal,low,4.00");
}

void calib_low_step2() {
    //get the reading from the device
    Serial.println("calib_low_step2");
    receive_and_print_response(pH_Device);             
    Serial.println("");
}

void calib_high_step1() {
    //send a read command
    Serial.println("calib_high_step1");
    pH_Device.send_cmd("cal,high,10.00");
}

void calib_high_step2() {
    //get the reading from the device
    Serial.println("calib_high_step2");
    receive_and_print_response(pH_Device);             
    Serial.println("");
}

void calib_check_step1() {
    //send a read command
    Serial.println("calib_check_step1");
    pH_Device.send_read_cmd();
}

void calib_check_step2() {
    //get the reading from the device
    Serial.println("calib_check_step2");
    receive_and_print_response(pH_Device);             
    Serial.println("");
}


void print_help() {
    Serial.println(F("Atlas Scientific I2C Scan Demo                                             "));
    Serial.println(F("Commands:                                                                  "));
    Serial.println(F("poll         Takes readings continuously of all sensors                    "));
    Serial.println(F("addr,[nnn]   Changes the sensors I2C address to number nnn                 "));
}

uint8_t me_ph() {
    static state_ph_t state_ph = READ_PH;

    switch(state_ph){
        case READ_PH:
            if(read_seq.run() == read_seq.FINISHED) {
                if(pH_Device.get_error() == pH::SUCCESS){
                    Serial.printf("Valor de pH: %.2f \n", pH_Device.get_last_received_reading());
                    return ME_FINISHED;
                } else {
                    Serial.println("ph read error");
                }
                state_ph = CALIB_PH;
                delay(1000);
            }
            break;
        case CALIB_PH:
            while(1); // lo cuelgo
            if(me_calib() == ME_FINISHED){
                state_ph = READ_PH;
                delay(1000);
            }
            break;

        default: //error
            state_ph = READ_PH;
            break;
    }
    return ME_PROCESSING;
}

uint8_t me_calib() {
    static state_calib_t state_calib = CLEAR_CALIB;

    switch(state_calib){
        case CLEAR_CALIB:
            if(calib_clear_seq.run() == calib_check_seq.FINISHED) {
                state_calib = MID_POINT;
            }
            break;
        case MID_POINT:
            if(calib_mid_seq.run() == calib_mid_seq.FINISHED) {
                state_calib = LOW_POINT;
            }
            break;

        case LOW_POINT:
            if(calib_low_seq.run() == calib_low_seq.FINISHED) {
                state_calib = HIGH_POINT;
            }
            break;

        case HIGH_POINT:
            if(calib_high_seq.run() == calib_high_seq.FINISHED) {
                state_calib = CHECK_CALIB;
            }
            break;

        case CHECK_CALIB:
            if(calib_check_seq.run() == calib_check_seq.FINISHED) {
                state_calib = CLEAR_CALIB;
                return ME_FINISHED;
            }
            break;

        default: //error
            state_calib = MID_POINT;
            break;
    }

    return ME_PROCESSING;
}