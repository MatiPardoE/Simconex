/**
 ******************************************************************************
 * @file           	: modbusGatewayCode.h
 * @author         	: Tobias Bavasso Piizzi
 * @brief			:
 * @date			: 2023/12/24
 * @version			: 1.00
 ******************************************************************************
 * @attention		: This file it's dependent from
 * 					  uc's family used
 *
 ******************************************************************************
 */

#ifndef API_MODBUSGATEWAYCODE_H_
#define API_MODBUSGATEWAYCODE_H_


/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <modbusGatewayApi.h>
#include <fw_all.h>
#include <MbedSW.h>

/***********************************************
 * @brief			: Macros
 **********************************************/
#define SYSTICK_msec(x)			(x*( ((CORE_FREQ/DIV_TOTAL)*1000) ))


/***********************************************
 * @brief			: Typedef
 **********************************************/

/*! @typedef
 * @brief 	:
 * @details	:
 *
 * @param
 * @param
 */

/***********************************************
 * @brief			: Function initialization prototype
 **********************************************/

/*RET	NAME			argN							*/

/* ----------------------------------------------------------------------------
   -- ESP32
   ---------------------------------------------------------------------------- */
extern void initEsp32GPIOS 		( void );
extern void initEsp32LPUART 	( void );

/* ----------------------------------------------------------------------------
   -- MODBUS
   ---------------------------------------------------------------------------- */
extern void initRdoGPIOS 	( void );
extern void initRdoLPUART 	( void );

/***********************************************
 * @brief			: Function project prototype
 **********************************************/

/*RET	NAME			argN							*/
extern void initGPIOS 			( void );
extern void initPERIPHERICS 	( void );
extern void initVARS 			( void );

extern void disablePERIPHERICS	( void );


/***********************************************
 * @brief			: Function from project used by pfoo
 **********************************************/

void sensorRdoTxEnable 		( void * args );
void sensorRdoTxDisable 	( void * args );

void espTxEnable 			( void * args );
void espTxDisable 			( void * args );

extern uint8_t rGpio	( __RW gpio_t * this , void * args );
extern uint8_t wGpio	( __RW gpio_t * this , uint8_t value , void * args );

#endif /* API_MODBUSGATEWAYCODE_H_ */
