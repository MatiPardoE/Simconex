
#include "S32K116.h"
#include <MbedSW.h>
#include <fw_all.h>

#define _TX_		TX_PORT,TX_GPIO
#define TX_GPIO		3
#define TX_PORT		PORT_A
#define TX_MUX		GPIO_MUX_ALT6

#define _RX_		RX_PORT,RX_GPIO
#define RX_GPIO		2
#define RX_PORT		PORT_A
#define RX_MUX		GPIO_MUX_ALT6

#define _DERE_		DERE_PORT,DERE_GPIO
#define DERE_GPIO	2
#define DERE_PORT	PORT_D
#define DERE_MUX	GPIO_MUX_GPIO

#define UART_BUFF_SZ 256
#define _UART_					UART0
#define _UART_IRQ_				LPUART0_RxTx_IRQn
#define _512000N1				8  , 10 , 0
#define _192000E1				24 , 100 , 0

#define SYSTICK_msec(x)			(x*( ((CORE_FREQ/DIV_TOTAL)*1000) ))
#define TICKS_SEND				1000
__RW time_t		tick_ms 	= {.ticks32b = 0,	.ticks16b = 0,	.ticks8b = 0};
////////////////////
//// 	UART	////
////////////////////
__RW circ_buff_t 	txCbuff;
__RW uint8_t		txBuff 		[UART_BUFF_SZ];
__RW circ_buff_t 	rxCbuff;
__RW uint8_t		rxBuff 		[UART_BUFF_SZ];
__RW msec16_t 		ticksSend 	= 0;
////////////////////
//// 	UART	////
////////////////////


/***********************************************************************************
  * @function 	LPUARTInit
  * @brief		Inits a lpuart for send data.
  *
  * @details
  * @param
  *
  * @retval 	void
 **********************************************************************************/
void LPUARTInit	( void ){
	LPUARTx_Init		( _UART_		, LPUART_EXT_CLK	, _192000E1  , LIN_LBKDIE_DIS , LPUART_TX_noINV , BREAK_CHAR_LEN9_LEN13 , LIN_BREAK_DETEC_DIS 	);	//! Habilito periférico
	IP_LPUART0->CTRL 	|=  LPUART_CTRL_M(1) |  LPUART_CTRL_PE(1) | LPUART_CTRL_PT(0); //para usar la paridad
	LPUARTx_TI			( _UART_		, TX_IRQ_DIS						);	/*Disable #TX IRQ @ BUFFER EMPTY*/
	LPUARTx_RI			( _UART_		, RX_IRQ_ENA						);	/*Disable #RX IRQ @ BUFFER FULL*/
	HandlerxInit		(_UART_IRQ_		, 0x00								);	/*Enable #IRQ @ CHn*/
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

void sendRequest ( __RW circ_buff_t * buff ){

	uint16_t crc;
	pushCircBuff(buff, 0x01);
	pushCircBuff(buff, 0x03);
	pushCircBuff(buff, 0x00);
	pushCircBuff(buff, 37);
	pushCircBuff(buff, 0x00);
	pushCircBuff(buff, 0x02);
	crc = calcular_crc_modbus(&buff->buff[buff->inx_out], 6 ); //debiera ser la +- de in out
	pushCircBuff(buff, crc & 0xFF);
	pushCircBuff(buff, (uint8_t)((crc>>8) & 0xFF));

#ifdef __S32K1XX__
	_SET_GPIO(DERE_PORT,DERE_GPIO);
	LPUARTx_TI(_UART_,1);
#endif

}


int main(void) {

	WDOGInit();	// !< Inicializacion del watchdog (Deshabilitarlo)
	ClockFIRC48MhzInit(); 								// !< Inicializo el FIRC a 48Mhz

	ClockNormalRun48MhzInit(); 							// !< Activo el modo Normal a 48Mhz
	ClockSysTickInit(SYSTICK_msec(1),1); 				// !< Habilito el SysTick
	ClockGpioInit();									// !< Habilito el Clockout @ GPIOx

	GpioConfigPin(		_TX_,		GPIO_INPUT,		GPIO_INT_DISABLE,		TX_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
	GpioConfigPin(		_RX_,		GPIO_INPUT,		GPIO_INT_DISABLE,		RX_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
	GpioConfigPin(		_DERE_,		GPIO_OUTPUT,	GPIO_INT_DISABLE,		GPIO_MUX_GPIO,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);

	////////////////////
	//// 	UART	////
	////////////////////
	txCbuff = newCircBuff( txBuff, UART_BUFF_SZ );	//! buffers @ UART/LIN
	rxCbuff = newCircBuff( rxBuff, UART_BUFF_SZ );	//! buffers @ UART/LIN
	////////////////////
	//// 	UART	////
	////////////////////

	_CLEAR_GPIO(DERE_PORT,DERE_GPIO);

	LPUARTInit();

    for (;;) {

    	//hacer función que envíe el paquete y escribir el handler
    	if( (msec16_t)(tick_ms.ticks16b - ticksSend) > TICKS_SEND){
    		_SET_GPIO(DERE_PORT,DERE_GPIO);
    		sendRequest( &txCbuff );
    		ticksSend = tick_ms.ticks16b;
    	}

    }

}

/***********************************************************************************
 * @function SysTick_Handler
 * @brief  Estructura systick 1 msegundo
 * @param  void
 * @retval void
 **********************************************************************************/
void SysTick_Handler(void) {
	tick_ms.ticks32b++;
	tick_ms.ticks16b++;
	tick_ms.ticks8b++;
}

/***********************************************************************************
  * @function 	LPUART1_RxTx_IRQHandler
  * @brief  	Handler para la Rx Tx de UART
  * @param  	void
  * @retval 	void
  **********************************************************************************/
void LPUART0_RxTx_IRQHandler(void){

	uint8_t uart_tx 	= 0;
	uint8_t uart_rx 	= 0;

	////////////////////////
	//// TRANSMISSION	////
	////////////////////////
	//! Transmission Interrupt Enable
	if( LPUARTx_CTRL_TIE(_UART_) &&  LPUARTx_TX_COMPLETE(_UART_) ){

		if( popCircBuff(&uart_tx, &txCbuff) != ERROR_VAL ){
#ifdef __S32K1XX__
			IP_LPUART0->DATA = uart_tx;
#endif
		}
		else{
#ifdef __S32K1XX__
			LPUARTx_TI(_UART_,0);
			_CLEAR_GPIO(DERE_PORT,DERE_GPIO);
#endif
		}

	}
	////////////////////////
	//// TRANSMISSION	////
	////////////////////////

	////////////////////////
	//// 	RECEPTION	////
	////////////////////////
	//! Receive data buffer full
	if( LPUARTx_RX_BUFFER_FULL(_UART_) ){
//		uart_rx = IP_LPUART0->DATA>>1;
		uart_rx = IP_LPUART0->DATA;
		pushCircBuff( &rxCbuff , uart_rx );
		return;
	}
	////////////////////////
	//// 	RECEPTION	////
	////////////////////////

}

