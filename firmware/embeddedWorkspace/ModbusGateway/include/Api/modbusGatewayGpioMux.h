/**
 ******************************************************************************
 * @file           	: modbusGatewayGpioMux.h
 * @author         	: Tobias Bavasso Piizzi
 * @brief			:
 * @date			: 2024/11/23
 * @version			: 1.00
 ******************************************************************************
 * @attention		: This file it's dependent from
 * 					  uc's family used
 * 					  Este archivo contiene las definiciones para usar los GPIOS
 *
 ******************************************************************************
 */

#ifndef API_RDOGATEWAYGPIOMUX_H_
#define API_RDOGATEWAYGPIOMUX_H_


/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <std/std.h>


/***********************************************
 * @brief			: Defines
 **********************************************/


/* ----------------------------------------------------------------------------
   -- RDO INTERFACE
   ---------------------------------------------------------------------------- */
#define _RDO_TX_						RDO_TX_PORT,RDO_TX_GPIO			//PTA3	; PIN35	; LPUART0_TX
#define RDO_TX_GPIO					3
#define RDO_TX_PORT					PORT_A
#define RDO_TX_MUX					GPIO_MUX_ALT6
//
#define _RDO_RX_						RDO_RX_PORT,RDO_RX_GPIO			//PTA3	; PIN35	; LPUART0_TX
#define RDO_RX_GPIO					2
#define RDO_RX_PORT					PORT_A
#define RDO_RX_MUX					GPIO_MUX_ALT6
//
#define _RDO_CTS_					RDO_CTS_PORT,RDO_CTS_GPIO			//PTA0	; PIN38	; PTA0
#define RDO_CTS_GPIO					0
#define RDO_CTS_PORT					PORT_A
#define RDO_CTS_MUX					GPIO_MUX_GPIO
//
#define _RDO_RTS_					RDO_RTS_PORT,RDO_RTS_GPIO			//PTA1	; PIN37	; PTA1
#define RDO_RTS_GPIO					1
#define RDO_RTS_PORT					PORT_A
#define RDO_RTS_MUX					GPIO_MUX_GPIO


/* ----------------------------------------------------------------------------
   -- ESP INTERFACE
   ---------------------------------------------------------------------------- */
#define _ESP32_TX_						ESP32_TX_PORT,ESP32_TX_GPIO			//PTC9	; PIN27	; LPUART1_TX
#define ESP32_TX_GPIO					9
#define ESP32_TX_PORT					PORT_C
#define ESP32_TX_MUX					GPIO_MUX_ALT2
//
#define _ESP32_RX_						ESP32_RX_PORT,ESP32_RX_GPIO			//PTC8	; PIN28	; LPUART1_RX
#define ESP32_RX_GPIO					8
#define ESP32_RX_PORT					PORT_C
#define ESP32_RX_MUX					GPIO_MUX_ALT2



#endif /* API_RDOGATEWAYGPIOMUX_H_ */
