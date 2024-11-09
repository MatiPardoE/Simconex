#include <Arduino.h>
//cosas RDO
#include <rdoApiCode.h>
#include <driver/uart.h>
#include <soc/uart_reg.h>
#include "hal/uart_ll.h"
#include "soc/interrupts.h"

#include "cBuff.h"


/* ----------------------------------------------------------------------------
  -- RDO
  ---------------------------------------------------------------------------- */
uint32_t lastMillisRDO = 0; //para que arranque de una
volatile rdo_t rdo;
bool initRDO = true;

const int uart_buffer_size = (1024 * 2);
QueueHandle_t uart_queue;
const char test_str []= {0x01,0x03,0x00,0x2D,0x00,0x08,0xD4,0x05};
#define BUF_SIZE                (255)
uint8_t* data = (uint8_t*) malloc(BUF_SIZE);

#define UART_BUFF_SZ 255
__RW uint8_t		  txBuff 		[UART_BUFF_SZ];
__RW uint8_t		  rxBuff 		[UART_BUFF_SZ];
//__RW circ_buff_t 	txCbuff;
  __RW circ_buff_t txCbuff = {
    .full =CB_NOT_FULL ,
    .inx_in = 0, 
    .inx_out = 0,
    .len = UART_BUFF_SZ,
    .buff = txBuff,
  };
//__RW circ_buff_t 	rxCbuff;
__RW circ_buff_t rxCbuff = {
    .full =CB_NOT_FULL ,
    .inx_in = 0, 
    .inx_out = 0,
    .len = UART_BUFF_SZ,
    .buff = txBuff,
  };

  uint32_t rise = 0;
  uint32_t fall = 0;

  /***********************************************************************************
  * @function 	pushCircBuff
  * @brief  	Guarda un valor en el buffer circular
  * @param  	cb 		: Pointer to circular buffer
  * @param  	src		: Value to store
  * @retval 	(0)=on pushed	;	(-1)=ERROR, buffer it's already full
  **********************************************************************************/
int8_t pushCircBuff	( __RW circ_buff_t *cb , uint8_t src){

	if(cb->full != CB_FULL){

		//! Store the value
		cb->buff[(cb->inx_in)++] = src;

		//! Check not to be the last
		if(cb->inx_in == cb->len)
			cb->inx_in = 0;

		//! Check not to be full for the next time
		if(cb->inx_in == cb->inx_out)
			cb->full = CB_FULL;
		else
			cb->full = CB_NOT_FULL;

		return(SUCCES_VAL);
	}
	else	//it's already full
		return(ERROR_VAL);

}

/***********************************************************************************
  * @function 	popRing
  * @brief  	Retira un valor en el buffer circular
  * @param  	dst		: Pointer where data out will be stored
  * @param  	cb 		: Pointer to circular buffer
  * @retval 	Value taken out	;	(-1)=(0xFFFFFFFF)ERROR, buffer it's already empty
  **********************************************************************************/
uint32_t popCircBuff ( uint8_t *dst	, __RW circ_buff_t *cb	){

	if(cb->full != CB_EMPTY){

		//! Prepare the value
		*dst = cb->buff[(cb->inx_out)++];

		//! Check not be the last
		if(cb->inx_out == cb->len)
			cb->inx_out = 0;

		//! Check not to be empty for the next time
		if(cb->inx_in == cb->inx_out)
			cb->full = CB_EMPTY;
		else
			cb->full = CB_NOT_FULL;

		return(*dst);
	}
	else
		return(ERROR_VAL);
}

static void IRAM_ATTR uart_intr_handle(void *arg)
{

  uint8_t tmp;
  uint16_t i;
  

  if( popCircBuff(&tmp, &txCbuff) != ERROR_VAL ){
    uart_write_bytes(RDO_UART, (const char*)&tmp, 1 );
    //Serial.printf("%d - %d",txCbuff.inx_out,txCbuff.inx_in);
    if(txCbuff.inx_out == txCbuff.inx_in){
      delayMicroseconds(518);
      digitalWrite(RDO_DE_RE_GPIO,0);
    }

  }
  /*
  else{
    uart_disable_intr_mask(RDO_UART,UART_TX_DONE_INT_ENA_M);
    //digitalWrite(RDO_DE_RE_GPIO, 0);
  }
  */

  // Obtener el estado de las interrupciones UART
  uint32_t uart_intr_status = UART1.int_st.val;  // Usamos UART0.int_st.val para obtener el estado de las interrupciones de UART0
  UART1.int_clr.tx_done = 1;


 return;

}

void setup() {
  pinMode(RDO_DE_RE_GPIO,OUTPUT);
  digitalWrite(RDO_DE_RE_GPIO,0);
  pinMode(RDO_TX_GPIO,OUTPUT);
  digitalWrite(RDO_TX_GPIO,0);
  

  clearRDO();



  const int uart_num = RDO_UART;
  uart_config_t uart_config = {
      .baud_rate = RDO_BAUD_RATE,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_EVEN,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 1,
      .source_clk = UART_SCLK_APB,
  };


  uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(uart_num, &uart_config);
  uart_set_pin(uart_num, RDO_TX_GPIO, RDO_RX_GPIO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  //uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX);
  uart_set_mode(uart_num, UART_MODE_UART);
  uart_set_rx_timeout(uart_num, 92);

  /*
  uart_intr_config_t intr_config = {
      .intr_enable_mask = UART_TX_DONE_INT_ENA_M,
  };

  uart_isr_free(uart_num);
  uart_isr_register(uart_num, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, NULL);
  uart_intr_config(uart_num, &intr_config);
  uart_disable_intr_mask(uart_num,UART_TX_DONE_INT_ENA_M);
  */

  digitalWrite(RDO_DE_RE_GPIO,1);
  delayMicroseconds(1000);
  digitalWrite(RDO_DE_RE_GPIO,0);
  delayMicroseconds(100);
  digitalWrite(RDO_DE_RE_GPIO,1);
  delayMicroseconds(500);
  digitalWrite(RDO_DE_RE_GPIO,0);
}

void loop() {
  
  
  if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
    _updateTimeout_;
    //requestRDO( &rdo );
    /*
    pushCircBuff(&txCbuff, 0x01);
	  pushCircBuff(&txCbuff, 0x03);
	  pushCircBuff(&txCbuff, 0x00);
	  pushCircBuff(&txCbuff, 0x2D);
	  pushCircBuff(&txCbuff, 0x00);
	  pushCircBuff(&txCbuff, 0x08);
    pushCircBuff(&txCbuff, 0xD4);
    pushCircBuff(&txCbuff, 0x05);
    uint8_t tmp;
    digitalWrite(RDO_DE_RE_GPIO,0);
    digitalWrite(RDO_DE_RE_GPIO,1);
    popCircBuff(&tmp, &txCbuff);
    */
    digitalWrite(RDO_DE_RE_GPIO,1);
    rise = micros();
    uart_write_bytes(RDO_UART, (const char*)&test_str, 8 );
    fall = rise + 8 * 11*52+ 8;
    /*
    uart_enable_intr_mask(RDO_UART,UART_TX_DONE_INT_ENA_M);
    //Serial.printf("%d - %d",txCbuff.inx_out,txCbuff.inx_in);
    */
  }

  if( micros() > fall && fall != rise ){
    digitalWrite(RDO_DE_RE_GPIO,0);
    rise = fall;
  }

}
