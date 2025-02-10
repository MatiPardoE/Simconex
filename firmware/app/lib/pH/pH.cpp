
#include "pH.h"
#include "Arduino.h"
#include "Wire.h"
#include <stdlib.h>

pH::pH(uint8_t address){
	this->i2c_address = address;
}

pH::pH(uint8_t address, const char* name){
	this->i2c_address = address;
	this->name = name;
}

pH::pH(uint8_t address, TwoWire* wire) : pH(address){
  this->wire = wire;
}

pH::pH(uint8_t address, const char* name, TwoWire* wire) : pH(address, name){
  this->wire = wire;
}

const char* pH::get_name(){
	return this->name;
}

void pH::set_name(const char* name){
	this->name = name;
}

uint8_t pH::get_address(){
    return i2c_address;
}

void pH::set_address(uint8_t address){
    this->i2c_address = address;
}

void pH::send_cmd(const char* command) {
  Wire.beginTransmission(this->i2c_address);
  #ifdef ESP32
  Wire.write((const uint8_t*)command, strlen(command)); 
  #else
  Wire.write(command); 
  #endif
  Wire.endTransmission();
  this->issued_read = false;
}

void pH::send_read_cmd(){
	send_cmd("r");
	this->issued_read = true;
}

void pH::send_cmd_with_num(const char* cmd, float num, uint8_t decimal_amount){
	String temp = String(cmd )+ String(num, (unsigned int)decimal_amount);
	const char* pointer = temp.c_str();
	send_cmd(pointer);
}

void pH::send_read_with_temp_comp(float temperature){
	send_cmd_with_num("rt,", temperature, 3);
	this->issued_read = true;
}


enum pH::errors pH::receive_read_cmd(){
	
	char _sensordata[this->bufferlen];
	this->error = receive_cmd(_sensordata, bufferlen);
	
	if(this->error == SUCCESS){
		if(this->issued_read == false){
			this->error = NOT_READ_CMD;
		}
		else{
			this->reading = atof(_sensordata);
		}
	}
	return this->error;
}

bool pH::is_read_poll(){
	return this->issued_read;
}

float pH:: get_last_received_reading(){  
	return this->reading;
}

enum pH::errors pH::get_error(){
	return this->error;
}

enum pH::errors pH::receive_cmd( char * sensordata_buffer, uint8_t buffer_len) {
  byte sensor_bytes_received = 0;
  byte code = 0;
  byte in_char = 0;

  memset(sensordata_buffer, 0, buffer_len);        // clear sensordata array;

  wire->requestFrom(this->i2c_address, (uint8_t)(buffer_len-1), (uint8_t)1);
  code = wire->read();

  //wire->beginTransmission(this->i2c_address);
  while (wire->available()) {
    in_char = wire->read();

    if (in_char == 0) {
      //wire->endTransmission();
      break;
    }
    else {
      sensordata_buffer[sensor_bytes_received] = in_char;
      sensor_bytes_received++;
    }
  }
  
  //should last array point be set to 0 to stop string overflows?
  switch (code) {
    case 1:
	  this->error = SUCCESS;
      break;

    case 2:
	  this->error = FAIL;
      break;

    case 254:
	  this->error = NOT_READY;
      break;

    case 255:
	  this->error = NO_DATA;
	  break;
  }
  return this->error;
  
}

bool pH::is_on_calibration()
{
    return this->on_calibration;
}

bool pH::start_calibration()
{
    if (!this->on_calibration)
    {
        this->on_calibration = true;
        return true;
    }else{
      // ya esta en modo calibracion
        return false;
    }
}

bool pH::finish_calibration()
{
    if (this->on_calibration)
    {
        this->on_calibration = false;
        this->state_calib = NO_CALIB;
        return true;
    }else{
      // No esta en modo calibracion  de pH
        return false;
    }
}
