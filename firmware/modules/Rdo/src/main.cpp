#include <Arduino.h>

//cosas RDO
#include <rdoApiCode.h>
//MODBUS
#include <ModbusMessage.h>
#include <esp32ModbusRTU.h>
#include <algorithm>  // for std::reverse

/* ----------------------------------------------------------------------------
  -- RDO
  ---------------------------------------------------------------------------- */
esp32ModbusRTU modbus(&Serial1, RDO_DE_RE_GPIO);  // use Serial1 and GPIO11/27 as RTS
uint32_t lastMillisRDO = 0; //para que arranque de una
volatile rdo_t rdo;
bool initRDO = true;
uint8_t calModeOn [] = { 0xE0, 0x00 };
uint8_t saturationBarometricPressure100 [] = { 0x44, 0x7D, 0x50, 0x00 };
uint8_t saturationSalinity100           [] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t defaultBarometricPressure       [] = { 0x44, 0x7D, 0x50, 0x00 };

uint8_t calUpdate [] = { 0xE0, 0x01 };
uint8_t calModeOff [] = { 0xE0, 0x02 };

//
uint32_t startCalib = 0;
bool first_time = true;

void setup() {


    Serial.begin(9600);  // Serial output
    Serial1.begin(RDO_BAUD_RATE, SERIAL_8E1, RDO_RX_GPIO, RDO_TX_GPIO, false);  // Modbus connection de libreria
    //Serial1.begin(RDO_BAUD_RATE, SERIAL_8E1, RDO_TX_GPIO, RDO_RX_GPIO, false);  // Modbus connection de libreria
    
    modbus.onData(rxRDO);
    modbus.onError(rxErrorRDO);
    modbus.begin();

    clearRDO();

    startCalib = millis();

}

void loop() {

    if(first_time){
        first_time = false;
        startCalib = millis();
    }



    if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
        requestRDO( &rdo );
        _updateTimeout_;
    }



    if(     ( (uint32_t)(millis() - startCalib) > (uint32_t)(15000) ) && rdo.onCalibration == false ){
        startCalib = millis();
        triggerPercentSaturationCalibration( &rdo );
        //startCalib = 0;
    }



}

