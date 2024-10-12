/* Copyright 2023 NXP */
/* License: BSD 3-clause
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "S32K116.h"
#include <MbedSW.h>
#include <fw_all.h>

#define _TX_		TX_PORT,TX_GPIO
#define TX_GPIO		3
#define TX_PORT		PORT_A
#define TX_MUX		GPIO_MUX_ALT3

#define _RX_		RX_PORT,RX_GPIO
#define RX_GPIO		2
#define RX_PORT		PORT_A
#define RX_MUX		GPIO_MUX_ALT3

#define _DERE_		DERE_PORT,DERE_GPIO
#define DERE_GPIO	2
#define DERE_PORT	PORT_D
#define DERE_MUX	GPIO_MUX_ALT3

#define UART_BUFF_SZ 256
#define _UART_					UART0
#define _UART_IRQ_				LPUART0_RxTx_IRQn
#define _512000N1				8  , 10 , 0
#define _192000E1				250 , 10 , 0

__RW time_t		tick_ms 	= {.ticks32b = 0,	.ticks16b = 0,	.ticks8b = 0};
////////////////////
//// 	UART	////
////////////////////
__RW circ_buff_t 	txCbuff;
__RW uint8_t		txBuff 		[];
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
	IP_LPUART0->CTRL 	|= LPUART_CTRL_PE(1) | LPUART_CTRL_PT(0); //para usar la paridad
	LPUARTx_TI			( _UART_		, TX_IRQ_DIS						);	/*Disable #TX IRQ @ BUFFER EMPTY*/
	LPUARTx_RI			( _UART_		, RX_IRQ_DIS						);	/*Disable #RX IRQ @ BUFFER FULL*/
	HandlerxInit		(_UART_IRQ_		, 0x00								);	/*Enable #IRQ @ CHn*/
}

void sendRequest ( __RW circ_buff_t * buff ){

	pushCircBuff(buff, '#');
	pushCircBuff(buff, centena);
	pushCircBuff(buff, decena);
	pushCircBuff(buff, unidad);
	pushCircBuff(buff, '&');

#ifdef __S32K1XX__
	LPUARTx_TI(_UART_,1);
#endif

}


int main(void) {

	WDOGInit();	// !< Inicializacion del watchdog (Deshabilitarlo)
	ClockFIRC48MhzInit(); 								// !< Inicializo el FIRC a 48Mhz

	ClockNormalRun48MhzInit(); 							// !< Activo el modo Normal a 48Mhz
	ClockSysTickInit(SYSTICK_msec(1),1); 				// !< Habilito el SysTick
	ClockGpioInit();									// !< Habilito el Clockout @ GPIOx

	GpioConfigPin(		_TX_,		GPIO_INPUT,		GPIO_INT_DISABLE,		LPUART_TX_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
	GpioConfigPin(		_RX_,		GPIO_INPUT,		GPIO_INT_DISABLE,		LPUART_RX_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
	GpioConfigPin(		_DERE_,		GPIO_OUTPUT,	GPIO_INT_DISABLE,		PB1_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);

	////////////////////
	//// 	UART	////
	////////////////////
	txCbuff = newCircBuff( txBuff, UART_BUFF_SZ );	//! buffers @ UART/LIN
	rxCbuff = newCircBuff( rxBuff, UART_BUFF_SZ );	//! buffers @ UART/LIN
	////////////////////
	//// 	UART	////
	////////////////////

	LPUARTInit();

    for (;;) {

    	//hacer función que envíe el paquete y escribir el handler
    	if( (msec16_t)(tick_ms.ticks16b - ticksSend) > TICKS_SEND_RPM ){
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
void LPUART1_RxTx_IRQHandler(void){

	uint8_t uart_tx 	= 0;

	////////////////////////
	//// TRANSMISSION	////
	////////////////////////
	//! Transmission Interrupt Enable
	if( LPUARTx_CTRL_TIE(_UART_) &&  LPUARTx_TX_COMPLETE(_UART_) ){

		if( popCircBuff(&uart_tx, &txCbuff) != ERROR_VAL ){
#ifdef __S32K1XX__
			LPUART1->DATA = uart_tx;
#endif
		}
		else{
#ifdef __S32K1XX__
			LPUARTx_TI(_UART_,0);
#endif
		}

	}
	////////////////////////
	//// TRANSMISSION	////
	////////////////////////

}

