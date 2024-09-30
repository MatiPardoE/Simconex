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
uint32_t lastMillisRDO = -_TIMEOUT_RDO_REQUEST_; //para que arranque de una
volatile rdo_t rdo;


void setup() {
  Serial.begin(9600);  // Serial output
  Serial1.begin(RDO_BAUD_RATE, SERIAL_8E1, RDO_RX_GPIO, RDO_TX_GPIO, true);  // Modbus connection de libreria
  
/*
  modbus.onData([](uint8_t serverAddress, esp32Modbus::FunctionCode fc, uint8_t* data, size_t length) {
    Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
    for (size_t i = 0; i < length; ++i) {
      Serial.printf("%02x", data[i]);
    }
    std::reverse(data, data + 4);  // fix endianness
    Serial.printf("\nval: %.2f", *reinterpret_cast<float*>(data));
    Serial.print("\n\n");
  });

  modbus.onError([](esp32Modbus::Error error) {
    Serial.printf("error: 0x%02x\n\n", static_cast<uint8_t>(error));
  });

  */
  modbus.onData(rxRDO);  // Pasas la función directamente
  modbus.onError(rxErrorRDO);
  modbus.begin();


  clearRDO();
 
}

void loop() {
  
  //PROBABLEMENTE HAYA QUE CHEQUEAR QUE NO SE ESTE ESPERANDO UNA RESPUESTA
  if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
    _updateTimeout_;
    //Serial.print("sending Modbus request...\n");
    //modbus.readHoldingRegisters(0x01,0x00,0x01);
    requestRDO( &rdo );

  }
  

}

