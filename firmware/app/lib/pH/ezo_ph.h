#include <Arduino.h>
#include <iot_cmd.h>
#include <pH_i2c_util.h> //brings in common print statements
#include <pH.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h> //include arduinos i2c library
#include <sequencer2.h> //imports a 2 function sequencer 
#include "esp_log.h"

#define I2C_SDA 12
#define I2C_SCL 14

#define READING_DELAY   1000
#define CLEAR_DELAY     400

bool init_pH_probe();
void print_help();
//uint8_t me_ph();
// uint8_t me_calib();
bool can_ph_read();
float get_ph();
bool clear_calib_ph_secuencer();
bool calib_mid_ph_sequencer();
bool calib_low_ph_sequencer();
bool calib_high_ph_sequencer();
bool calib_check_ph_sequencer();
bool comp_temp_calib_ph_sequencer();
bool is_comm_clear();

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
void comp_temp_step1();
void comp_temp_step2();