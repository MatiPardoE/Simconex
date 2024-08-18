#include <iot_cmd.h>
#include <pH_i2c_util.h> //brings in common print statements
#include <pH.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h> //include arduinos i2c library
#include <sequencer2.h> //imports a 2 function sequencer 

#define I2C_SDA 12
#define I2C_SCL 14

#define READING_DELAY   1000
#define CLEAR_DELAY     400

#define ME_IN_PROGRESS 0
#define ME_FINISHED    1

enum state_calib_t {
    CLEAR_CALIB,
    MID_POINT,
    LOW_POINT,
    HIGH_POINT,
    CHECK_CALIB
};

enum state_ph_t {
    READ_PH,
    CALIB_PH
};

pH pH_Device = pH(20, "EZO pH probe");   //object which holds the device we'll find

bool polling = true; //variable to determine whether or not were polling the circuits

void read_step1();     //forward declarations of functions to use them in the sequencer before defining them
void read_step2();
void calib_clear_step1();
void calib_clear_step2();
void calib_mid_step1();
void calib_mid_step2();
void calib_low_step1();
void calib_low_step2();
void calib_high_step1();
void calib_high_step2();
void calib_check_step1();
void calib_check_step2();

void print_help();
uint8_t me_calib();
void me_ph();

Sequencer2 read_seq(&read_step1, READING_DELAY,   //calls the steps in sequence with time in between them
                    &read_step2, 0 );

Sequencer2 calib_clear_seq(&calib_clear_step1, CLEAR_DELAY,   //calls the steps in sequence with time in between them
                           &calib_clear_step2, 0 );

Sequencer2 calib_mid_seq(&calib_mid_step1, READING_DELAY,   //calls the steps in sequence with time in between them
                         &calib_mid_step2, 0 );

Sequencer2 calib_low_seq(&calib_low_step1, READING_DELAY,   //calls the steps in sequence with time in between them
                         &calib_low_step2, 0 );

Sequencer2 calib_high_seq(&calib_high_step1, READING_DELAY,   //calls the steps in sequence with time in between them
                          &calib_high_step2, 0 );

Sequencer2 calib_check_seq(&calib_check_step1, CLEAR_DELAY,   //calls the steps in sequence with time in between them
                           &calib_check_step2, 0 );

bool init_pH_probe(){
    Wire.begin(I2C_SDA, I2C_SCL); //start the I2C
    delay(3000); //wait for devices to boot
    Wire.beginTransmission(PH_ADDR); //try to talk to the device over I2C
    if(Wire.endTransmission() == 0) { //check if communication attempt was successful   
        pH_Device.set_address(PH_ADDR); //set that to be the address
        pH_Device.send_cmd("I"); //ask what kind of device it is
        delay(300);
        receive_and_print_response(pH_Device); //print the response
        return true;
    }
    return false;
}

void setup() {
    Serial.begin(9600); //Set the hardware serial port to 9600
    init_pH_probe();
    Serial.println();
    read_seq.reset(); //initialize the sequencer
}

void loop() {
    String cmd; //variable to hold commands we send to the kit
    me_ph();
    //read_seq.run();
}

void read_step1() {
    //send a read command
    Serial.println("read_step1");
    pH_Device.send_read_cmd();
}

void read_step2() {
    //get the reading from the device
    Serial.println("read_step2");
    receive_and_print_reading(pH_Device);             
    Serial.println("");
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

void me_ph() {
    static state_ph_t state_ph = READ_PH;

    switch(state_ph){
        case READ_PH:
            if(read_seq.run() == read_seq.FINISHED) {
                state_ph = CALIB_PH;
                delay(1000);
            }
            break;
        case CALIB_PH:
            if(me_calib() == ME_FINISHED){
                state_ph = READ_PH;
                delay(1000);
            }
            break;

        default: //error
            state_ph = READ_PH;
            break;
    }
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

    return ME_IN_PROGRESS;
}