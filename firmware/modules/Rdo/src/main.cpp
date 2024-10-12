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


void setup() {
  Serial.begin(9600);  // Serial output
  Serial1.begin(RDO_BAUD_RATE, SERIAL_8E1, RDO_RX_GPIO, RDO_TX_GPIO, false);  // Modbus connection de libreria
  
  modbus.onData(rxRDO);
  modbus.onError(rxErrorRDO);
  modbus.begin();

  clearRDO();

  pinMode(RDO_LED1,OUTPUT);
  pinMode(RDO_LED2,OUTPUT);
  pinMode(RDO_LED3,OUTPUT);
  pinMode(RDO_LED4,OUTPUT);

}

void loop() {
  /*
  if( initRDO == false){
    if ( _TIMEOUT_TO_RDO_INIT_ ) {
      requestRDO( &rdo );
      _updateTimeout_;
    }
    if( rdo.headers.serialNumber != 0){
      initRDO = true;
      _updateTimeout_;
    }
  }
  else if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
    requestRDO( &rdo );
    _updateTimeout_;
  }
  */
  if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
    requestRDO( &rdo );
    _updateTimeout_;
  }
  

}

