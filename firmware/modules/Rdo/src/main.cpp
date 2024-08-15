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

  // put your main code here, to run repeatedly:
  digitalWrite(RDO_DE_RE_GPIO, HIGH);
  uart_write_bytes( RDO_UART_NUM, (const char *)&test , sizeof(test) );
  delay(50);
  digitalWrite(RDO_DE_RE_GPIO, LOW);
  delay(50);

  /*
  digitalWrite(RDO_DE_RE_GPIO, HIGH);
  delay(1000);
  digitalWrite(RDO_DE_RE_GPIO, LOW);
  delay(1000);
  */
}

