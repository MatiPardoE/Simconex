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
  //Serial1.begin(RDO_BAUD_RATE, SERIAL_8E1, RDO_TX_GPIO, RDO_RX_GPIO, false);  // Modbus connection de libreria
  
  modbus.onData(rxRDO);
  modbus.onError(rxErrorRDO);
  modbus.begin();

  clearRDO();

}

void loop() {

  if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
    requestRDO( &rdo );
    _updateTimeout_;
  }
  

}

