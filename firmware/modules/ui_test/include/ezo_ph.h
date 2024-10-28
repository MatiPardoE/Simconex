#include <Arduino.h>
#include <iot_cmd.h>
#include <pH_i2c_util.h> //brings in common print statements
#include <pH.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h> //include arduinos i2c library
#include <sequencer2.h> //imports a 2 function sequencer 
#include "utilities.h"

#define I2C_SDA 12
#define I2C_SCL 14

#define READING_DELAY   1000
#define CLEAR_DELAY     400

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

bool init_pH_probe();
void print_help();
uint8_t me_ph();
uint8_t me_calib();

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