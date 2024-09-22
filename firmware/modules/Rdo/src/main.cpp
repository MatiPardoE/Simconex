#include <Arduino.h>
//cosas RDO
#include <rdoApi.h>

// Definir SERIAL_PORT_HARDWARE para el ESP32
#include <ArduinoRS485.h>
#include <ModbusMaster.h>


#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"


#include <soc/uart_struct.h>
#include <soc/uart_reg.h>



/* ----------------------------------------------------------------------------
   -- RDO
   ---------------------------------------------------------------------------- */

volatile bool uart_tx_done = false;
volatile bool uart_rx_ready = false;
const char test_str[] = {0xAA,0xF0};

// ISR para la interrupción de UART
static void IRAM_ATTR uart_isr_handler(void* arg) {
    uint32_t uart_intr_status = UART1.int_st.val; // UART1 es para UART_NUM_1
    digitalWrite(LED_BUILTIN, 1);


    // Interrupción de transmisión completada
    if (uart_intr_status & UART_TX_DONE_INT_ST_M) {
        uart_clear_intr_status(RDO_UART_NUM, UART_TX_DONE_INT_CLR_M);
        uart_tx_done = true;
        digitalWrite(RDO_DE_RE_GPIO, 1);
    }

    // Interrupción de recepción de datos
    if (uart_intr_status & UART_RXFIFO_FULL_INT_ST_M) {
        uart_clear_intr_status(RDO_UART_NUM, UART_RXFIFO_FULL_INT_CLR_M);
        uart_rx_ready = true;
    }
}


void setup() {

 // Configuración de pines UART
  gpio_set_direction(RDO_TX_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(RDO_RX_GPIO, GPIO_MODE_INPUT);
  gpio_set_pull_mode(RDO_RX_GPIO, GPIO_PULLUP_ONLY);

  pinMode(RDO_DE_RE_GPIO, OUTPUT);
  digitalWrite(RDO_DE_RE_GPIO, 0);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  // Configuración de UART
  uart_config_t uart_config = {
    .baud_rate  = RDO_BAUD_RATE,
    .data_bits  = UART_DATA_8_BITS,
    .parity     = RDO_PARITY,
    .stop_bits  = UART_STOP_BITS_1,
    .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB,
  };

  uart_param_config(RDO_UART_NUM, &uart_config);
  uart_set_pin(RDO_UART_NUM, RDO_RX_GPIO, RDO_RX_GPIO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
 
  // Instalar el controlador UART sin buffer para manejo por interrupciones
  uart_driver_install(RDO_UART_NUM, 1024, 1024, 0, NULL, 0);

  // Configurar las interrupciones UART
  uart_enable_intr_mask(RDO_UART_NUM, UART_TX_DONE_INT_ENA_M | UART_RXFIFO_FULL_INT_ENA_M);
  uart_isr_register(RDO_UART_NUM, uart_isr_handler, NULL, ESP_INTR_FLAG_IRAM, NULL);

  // Enviar mensaje inicial

  uart_tx_done = false;
  uart_write_bytes(RDO_UART_NUM, test_str, strlen(test_str));

}

void loop() {


  while(1){


  if (uart_tx_done) {
        uart_tx_done = false;
        uart_write_bytes(RDO_UART_NUM, test_str, strlen(test_str));
        digitalWrite(RDO_DE_RE_GPIO, 0);
    }

    // Procesar datos recibidos si hay
    if (uart_rx_ready) {
        uart_rx_ready = false;
    }
/*
    digitalWrite(RDO_DE_RE_GPIO, 0);
    delay(100);
    digitalWrite(RDO_DE_RE_GPIO, 1);
    delay(100);
*/
  }

}

