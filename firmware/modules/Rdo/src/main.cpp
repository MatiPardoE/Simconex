#include <Arduino.h>
//cosas RDO
#include <rdoApi.h>

// Definir SERIAL_PORT_HARDWARE para el ESP32
#include <ArduinoRS485.h>
#include <ModbusMaster.h>



/* ----------------------------------------------------------------------------
   -- RDO
   ---------------------------------------------------------------------------- */
// instantiate ModbusMaster object
ModbusMaster node;

void preTransmission()
{
  digitalWrite(RDO_DE_RE_GPIO, 1);
}

void postTransmission()
{
  digitalWrite(RDO_DE_RE_GPIO, 0);
}

// Función para calcular el CRC-16
uint16_t calculateCRC(uint8_t *data, uint8_t length) {
  uint16_t crc = 0xFFFF;

  for (uint8_t i = 0; i < length; i++) {
    crc ^= (uint16_t)data[i];  // XOR entre el byte actual y el CRC

    for (uint8_t j = 0; j < 8; j++) {  // Procesar cada bit del byte
      if (crc & 0x0001) {  // Si el bit menos significativo es 1
        crc >>= 1;
        crc ^= 0xA001;  // XOR con el polinomio
      } else {
        crc >>= 1;  // Desplazar el CRC a la derecha
      }
    }
  }
  
  return crc;
}


void setup() {

  pinMode(RDO_DE_RE_GPIO, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(RDO_DE_RE_GPIO, 0);
  digitalWrite(LED_BUILTIN, 0);

  
  Serial.begin(9600);

  // use Serial (port 1); initialize Modbus communication baud rate
  Serial1.begin(19200, SERIAL_8E1);


  /*
  // communicate with Modbus slave ID 2 over Serial (port 0)
  node.begin(1, Serial1);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  */

}

void loop() {

  uint8_t result;
  uint16_t data[6];

  int16_t rdoLengthRx;

  uint8_t slaveID  = 0x01;
  uint8_t funCode  = 0x03;
  uint8_t regMSB   = 0x00;
  uint8_t regLSB   = 0x01;
  uint8_t qReadMSB = 0x00;
  uint8_t qReadLSB = 0x01;
  //uint8_t modbusTx[] = { slaveID , funCode , regLSB , regMSB , qReadLSB , qReadMSB , 0 , 0 };
  uint8_t modbusTx[] = { slaveID , funCode , regMSB , regLSB , qReadMSB , qReadLSB , 0 , 0 };
  uint16_t crc;
  uint8_t i;
  // id cf dataB data
  uint8_t modbusRx[] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 };

  uint16_t rx = 0;


  while(1){
    digitalWrite(LED_BUILTIN, 1);
    delay(1000);

    crc = calculateCRC(modbusTx,6);
    modbusTx[6] = (crc>>8)&0xFF;
    modbusTx[7] = (crc)&0xFF;

    digitalWrite(RDO_DE_RE_GPIO, HIGH);
    
    for( i=0 ; i<8 ; i++){
      if( uart_write_bytes( RDO_UART_NUM, (const char *)&modbusTx[i] , sizeof(uint8_t) ) == '1')
        Serial.print("\n\rERROR!");
      uart_wait_tx_done(RDO_UART_NUM, portMAX_DELAY);
      //delay(1);
    }
    // Esperar a que la transmisión termine
    
    
    // Ahora que todos los bytes han sido enviados, puedes desactivar la transmisión
    digitalWrite(RDO_DE_RE_GPIO, LOW);

    //delay(100);
    //digitalWrite(RDO_DE_RE_GPIO, LOW);

    rdoLengthRx = uart_read_bytes(RDO_UART_NUM, (void*)modbusRx, 7, 100);
    Serial.print("\n\rESP32 received: ");
    Serial.println( rdoLengthRx );
    for( i=0 ; i<7 ; i++){
      Serial.println( modbusRx[i] );
    }

  /*
    result = node.readHoldingRegisters(0x0001, 2);
    if (result == node.ku8MBSuccess)
    {
      Serial.print("\n\rResponse: ");
      Serial.println(node.getResponseBuffer(0x04));
    }
    else{
      Serial.print("\n\rNo response.");
      Serial.print(result);
    }
  */

    digitalWrite(LED_BUILTIN, 0);
    delay(1000);
  }
  
  

}

