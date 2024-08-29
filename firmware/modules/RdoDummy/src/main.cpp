#include <Arduino.h>
//cosas RDO
#include <rdoApi.h>


/* ----------------------------------------------------------------------------
   -- RDO
   ---------------------------------------------------------------------------- */
uart_config_t rdoUARTconfig;
__RW uint8_t* rdoDataRx;

char test = 0x71;


void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);

  driverUartRDO();
  initUartRDO();
  initGpiosRDO();
  pinMode(RDO_DE_RE_GPIO, OUTPUT);

  rdoDataRx = (uint8_t*) malloc(RDO_BUFFER_SIZE);
}

void loop() {

  int16_t rdoLengthRx;

  uart_write_bytes( RDO_UART_NUM, (const char *)&test , sizeof(test) );
  do{
    rdoLengthRx = uart_read_bytes(RDO_UART_NUM, (void*)rdoDataRx, RDO_BUFFER_SIZE, 100);
    if( *rdoDataRx == 0x71 ){
      digitalWrite(RDO_DE_RE_GPIO, HIGH);
      test = 0x72;
    }
    else{
      digitalWrite(RDO_DE_RE_GPIO, LOW);
      test = 0x71;
    }
  }while( rdoLengthRx == 0); 
  delay(50);

  // put your main code here, to run repeatedly:
  /*
  digitalWrite(RDO_DE_RE_GPIO, HIGH);
  uart_write_bytes( RDO_UART_NUM, (const char *)&test , sizeof(test) );
  delay(50);
  digitalWrite(RDO_DE_RE_GPIO, LOW);
  delay(50);
  */

  /*
  digitalWrite(RDO_DE_RE_GPIO, HIGH);
  delay(1000);
  digitalWrite(RDO_DE_RE_GPIO, LOW);
  delay(1000);
  */
}

