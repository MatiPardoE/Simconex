#include <Arduino.h>

//cosas RDO
#include <rdoApiCode.h>
//MODBUS
#include <ModbusMessage.h>
#include <esp32ModbusRTU.h>
#include <algorithm>  // for std::reverse
#include <hal/uart_ll.h>
#include "driver/uart.h"
#include "soc/uart_struct.h"  // Estructuras de bajo nivel UART
#include "soc/uart_reg.h"     // Registros de la UART

/* ----------------------------------------------------------------------------
  -- RDO
  ---------------------------------------------------------------------------- */
esp32ModbusRTU modbus(&Serial1, RDO_DE_RE_GPIO);  // use Serial1 and GPIO11/27 as RTS
uint32_t lastMillisRDO = 0; //para que arranque de una
volatile rdo_t rdo;
bool initRDO = true;

#define BUF_SIZE                (127)
const int uart_num = UART_NUM_2;
    uart_config_t uart_config = {
        .baud_rate = RDO_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 1,
        .source_clk = UART_SCLK_APB,
        
    };




#define FRAME_TX_LENGHT 8
uint8_t frameTx [FRAME_TX_LENGHT] = { RDO_SLAVE_ID , 3 , 0x00 , _MEASURED_VALUE_DO_ , 0 , 2 , 0 , 0};
uint8_t ledActual ;
// Variable para almacenar los datos que deseas transmitir
volatile uint8_t *data_to_send;
volatile size_t data_length = 0;

// Prototipo de la ISR
static void IRAM_ATTR uart_tx_isr(void* arg);

// Implementación de la ISR
static void IRAM_ATTR uart_tx_isr(void* arg) {
    Serial.printf("HOLA!\n"); // Para verificar si se ejecuta

    // Verifica si hay datos para transmitir
    if (data_length > 0) {
        // Verifica si el buffer TX de la UART tiene espacio disponible
        if (uart_ll_get_txfifo_len(&UART2) > 0) {
            // Escribe directamente el byte en el registro del TX FIFO
            UART2.fifo.rw_byte = *data_to_send;
            data_to_send++;  // Avanza al siguiente byte
            data_length--;    // Decrementa la longitud restante
        }
    } else {
        // Cuando ya no hay más datos que enviar, deshabilita la interrupción TX
        if (data_length == 0){
          uart_disable_tx_intr(uart_num);
          Serial.printf("HOLA2!\n"); // Para verificar si se ejecuta
        }
    }
}

uint16_t calcular_crc_modbus(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;  // Valor inicial del CRC
    uint16_t i, j;

    for (i = 0; i < length; i++) {
        crc ^= data[i];  // XOR entre el byte de datos y el CRC

        for (j = 0; j < 8; j++) {  // Procesa cada bit
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;  // Polinomio para Modbus CRC-16
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}


// Función para enviar una cadena de bytes
void uart_send_data(const uint8_t *data, size_t length) {
    data_to_send = (volatile uint8_t*)(data);
    data_length = length;
    UART2.fifo.rw_byte = *data_to_send;
    uart_enable_tx_intr(uart_num, 1, 0); // Habilitar interrupción TX

}



void setup() {
  Serial.begin(9600);  // Serial output
  
  uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(uart_num, &uart_config);
  uart_set_pin(uart_num, RDO_TX_GPIO, RDO_RX_GPIO, RDO_DE_RE_GPIO, UART_PIN_NO_CHANGE);
  //uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX);
  //uart_set_rx_timeout(uart_num, 3);
  //
  uart_isr_register(uart_num, uart_tx_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);


  uint16_t crc = calcular_crc_modbus(frameTx,FRAME_TX_LENGHT-2);
  frameTx[FRAME_TX_LENGHT-2] = crc & 0xFF;
  frameTx[FRAME_TX_LENGHT-1] = (uint8_t)((crc>>8) & 0xFF);


  pinMode(RDO_LED1,OUTPUT);
  pinMode(RDO_LED2,OUTPUT);
  pinMode(RDO_LED3,OUTPUT);
  pinMode(RDO_LED4,OUTPUT);

}

void loop() {
  

 if ( _TIMEOUT_TO_RDO_REQUEST_ ){
  uart_send_data( frameTx,sizeof(frameTx) );
  ledActual = digitalRead(RDO_LED1);
  digitalWrite(RDO_LED1,!ledActual);
  _updateTimeout_;
 }


}



