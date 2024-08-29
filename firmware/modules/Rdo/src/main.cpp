#include <Arduino.h>
//cosas RDO
#include <rdoApi.h>

#include <ArduinoModbus.h>


/* ----------------------------------------------------------------------------
   -- RDO
   ---------------------------------------------------------------------------- */

uart_config_t rdoUARTconfig;
__RW uint8_t* rdoDataRx;
char test = 0x71;

constexpr auto baudrate { 19200 };





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
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Modbus Temperature Humidity Sensor");
  // start the Modbus RTU client
  if (!ModbusRTUClient.begin(baudrate,SERIAL_8E1)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1);
  }
  /*
  
  pinMode(LED_BUILTIN, OUTPUT);

  driverUartRDO();
  initUartRDO();
  initGpiosRDO();
  pinMode(RDO_DE_RE_GPIO, OUTPUT);
  rdoDataRx = (uint8_t*) malloc(RDO_BUFFER_SIZE);

  Serial.begin(9600);
  */

}

void loop() {

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

/*
    // send a Holding registers read request to (slave) id 1, for 2 registers
    if (!ModbusRTUClient.requestFrom(1, HOLDING_REGISTERS, 0x00, 1)) {
      Serial.print("failed to read registers! ");
      Serial.println(ModbusRTUClient.lastError());
    } else {
      // If the request succeeds, the sensor sends the readings, that are
      // stored in the holding registers. The read() method can be used to
      // get the raw temperature and the humidity values.
      rx = ModbusRTUClient.read();

      

      Serial.println(rx);
    }
*/
    rx = ModbusRTUClient.coilRead(0x01, 0x00);

    Serial.println(rx);

    delay(500);

    /*
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

    crc = calculateCRC(modbusTx,6);
    modbusTx[6] = (crc>>8)&0xFF;
    modbusTx[7] = (crc)&0xFF;

    digitalWrite(RDO_DE_RE_GPIO, HIGH);
    for( i=0 ; i<8 ; i++){
      uart_write_bytes( RDO_UART_NUM, (const char *)&modbusTx[i] , sizeof(uint8_t) );
      //delay(1);
    }
    delay(5);
    digitalWrite(RDO_DE_RE_GPIO, LOW);

    rdoLengthRx = uart_read_bytes(RDO_UART_NUM, (void*)modbusRx, 7, 100);


    Serial.print("\n\rESP32 received: ");
    Serial.println( rdoLengthRx );
    for( i=0 ; i<7 ; i++){
      Serial.println( modbusRx[i] );
    }
    
    if( modbusRx[5] == 42 ){
        digitalWrite(LED_BUILTIN, HIGH);
    }


    delay(1000);
    */
  }

}

