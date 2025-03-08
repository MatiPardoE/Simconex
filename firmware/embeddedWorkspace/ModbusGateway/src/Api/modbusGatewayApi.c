/**
 ******************************************************************************
 * @file           	: modbusGatewayApi.c
 * @author         	: Tobias Bavasso Piizzi
 * @brief			:
 * @date			: 2024/11/26
 * @version			: 1.00
 ******************************************************************************
 * @attention		:
 *
 *
 ******************************************************************************
 */

/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <modbusGatewayApi.h>

/***********************************************
 * @brief			: vars
 **********************************************/
__RW msec_t		ticks;
msec_t lastValidRequestFromEsp;
msec_t lastValidRequestFromRdo;

__RW MODBUS_t 	sensorRdo;
__RW gpio_t 	ctsRdo;
__RW gpio_t 	rtsRdo;

__RW MODBUS_t 	esp;



/***********************************************
 * @brief			: Function's  CODE
 **********************************************/


/* ----------------------------------------------------------------------------
   -- ESP32
   ---------------------------------------------------------------------------- */
/***********************************************************************************
  * @function initRdoGPIOS
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void initEsp32GPIOS ( void ){
	GpioConfigPin(		_ESP32_TX_,		GPIO_INPUT	,		GPIO_INT_DISABLE,		ESP32_TX_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
	GpioConfigPin(		_ESP32_RX_,		GPIO_INPUT	,		GPIO_INT_DISABLE,		ESP32_RX_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
}

/***********************************************************************************
  * @function initEsp32LPUART
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void initEsp32LPUART 	( void ){
	LPUARTx_Init		( _ESP_INIT_	);	//! Habilito periférico
	IP_LPUART1->CTRL 	|=  _PARITY_EVEN_; //para usar la paridad
	LPUARTx_TI			( ESP_UARTx		, TX_IRQ_DIS	);	/*Disable #TX IRQ @ BUFFER EMPTY*/
	LPUARTx_RI			( ESP_UARTx		, RX_IRQ_ENA	);	/*Disable #RX IRQ @ BUFFER FULL*/
	HandlerxInit		( ESP_IRQ		, 0x00			);	/*Enable #IRQ @ CHn*/
}

/* ----------------------------------------------------------------------------
   -- RDO
   ---------------------------------------------------------------------------- */
/***********************************************************************************
  * @function initRdoGPIOS
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void initRdoGPIOS ( void ){
	GpioConfigPin(		_RDO_TX_,		GPIO_INPUT	,		GPIO_INT_DISABLE,		RDO_TX_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
	GpioConfigPin(		_RDO_RX_,		GPIO_INPUT	,		GPIO_INT_DISABLE,		RDO_RX_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
	GpioConfigPin(		_RDO_CTS_,		GPIO_OUTPUT	,		GPIO_INT_DISABLE,		RDO_CTS_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
	GpioConfigPin(		_RDO_RTS_,		GPIO_OUTPUT	,		GPIO_INT_DISABLE,		RDO_RTS_MUX,	GPIO_LOWDRIVE,	GPIO_FILTER_OFF,	GPIO_PULL_DISABLE,	GPIO_PULL_DOWN);
}

/***********************************************************************************
  * @function initRdoLPUART
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void initRdoLPUART 	( void ){
	LPUARTx_Init		( _RDO_INIT_	);	//! Habilito periférico
	IP_LPUART0->CTRL 	|=  _PARITY_EVEN_; //para usar la paridad
	LPUARTx_TI			( RDO_UARTx		, TX_IRQ_DIS	);	/*Disable #TX IRQ @ BUFFER EMPTY*/
	LPUARTx_RI			( RDO_UARTx		, RX_IRQ_DIS	);	/*Disable #RX IRQ @ BUFFER FULL*/
	HandlerxInit		( RDO_IRQ		, 0x00			);	/*Enable #IRQ @ CHn*/
}

/***********************************************
 * @brief			: Function project prototype
 **********************************************/
/***********************************************************************************
  * @function initGPIOS
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void initGPIOS ( void ){
	initEsp32GPIOS();
	initRdoGPIOS();
}

/***********************************************************************************
  * @function initPERIPHERICS
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void initPERIPHERICS ( void ){
	initEsp32LPUART();
	initRdoLPUART();

}

/***********************************************************************************
  * @function initPERIPHERICS
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void initVARS ( void ){
	ctsRdo = configureGpio( _RDO_CTS_ , &rGpio , &wGpio );
	rtsRdo = configureGpio( _RDO_RTS_ , &rGpio , &wGpio );

	sensorRdo 	= modbus(	sensorRdoTxEnable ,sensorRdoTxDisable ,
							&ticks	, DELTA_TX_TICKS,
							MCU_RDO_TX , MCU_RDO_RX ,
							(gpio_t*)&ctsRdo , (gpio_t*)&rtsRdo
						);

	sensorRdo.request.rts->writeGpio( sensorRdo.request.rts , 0 , NULL );
	sensorRdo.request.cts->writeGpio( sensorRdo.request.cts , 0 , NULL );

	esp 		= modbus(	espTxEnable , espTxDisable ,
							&ticks	, DELTA_TX_TICKS,
							MCU_ESP_TX , MCU_ESP_RX,
							NULL , NULL
						);
}

/***********************************************************************************
  * @function disablePERIPHERICS
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void disablePERIPHERICS	( void ){
	ClockSysTickInit(SYSTICK_msec(1),0);//! apago por las dudas
	LPUARTx_Init		( _ESP_OFF_	);	//! apago por las dudas
	LPUARTx_Init		( _RDO_OFF_	);	//! apago por las dudas
}

/***********************************************
 * @brief			: Function from project used by pfoo
 **********************************************/
/***********************************************************************************
  * @function sensorRdoTxEnable
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void sensorRdoTxEnable 		( void * args ){
#ifdef __S32K1XX__
	LPUARTx_TI(RDO_UARTx,1);
#endif
}

/***********************************************************************************
  * @function sensorRdoTxDisable
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void sensorRdoTxDisable 	( void * args ){
#ifdef __S32K1XX__
	LPUARTx_TI(RDO_UARTx,0);
#endif
}

/***********************************************************************************
  * @function espTxEnable
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void espTxEnable 			( void * args ){
#ifdef __S32K1XX__
	LPUARTx_TI(ESP_UARTx,1);
#endif
}

/***********************************************************************************
  * @function espTxDisable
  * @brief
  * @param  	node
  * @retval 	none
  **********************************************************************************/
void espTxDisable 			( void * args ){
#ifdef __S32K1XX__
	LPUARTx_TI(ESP_UARTx,0);
#endif
}

/***********************************************************************************
  * @function	rGpio
  * @brief		Funcion generica para leer un gpio
  * 			en este proyecto
  * @details	es la forma de conectar la mbed con mi HAL
  * @param  	this : puntero al objeto gpio
  * @retval 	None
  **********************************************************************************/
uint8_t rGpio	( __RW gpio_t * this , void * args ){
#ifdef __S32K1XX__
	uint8_t tmp = _GET_GPIO( this->port , this->pin);
#endif
	this->lastValue = tmp;
	return( this->lastValue );
}

/***********************************************************************************
  * @function	wGpio
  * @brief		Funcion generica para escribir un gpio
  * 			en este proyecto
  * @details	es la forma de conectar la mbed con mi HAL
  * @param  	this : puntero al objeto gpio
  * @param  	value: nuevo valor a cargar
  * @retval 	None
  **********************************************************************************/
uint8_t wGpio	( __RW gpio_t * this , uint8_t value , void * args ){
	this->lastValue = value;
#ifdef __S32K1XX__
	_WRITE_GPIO( this->port , this->pin , this->lastValue );
#endif
	return( this->lastValue );
}


