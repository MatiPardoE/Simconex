/**
 ******************************************************************************
 * @file           	: modbusGatewayPeripherics.h
 * @author         	: Tobias Bavasso Piizzi
 * @brief			:
 * @date			: 2024/11/23
 * @version			: 1.00
 ******************************************************************************
 * @attention		: This file it's dependent from
 * 					  uc's family used
 * 					  Este archivo contiene las definiciones para usar los PERIFERICOS
 *
 ******************************************************************************
 */

#ifndef API_RDOGATEWAYPERIPHERICS_H_
#define API_RDOGATEWAYPERIPHERICS_H_

/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <std/std.h>
#include <fw_all.h>

/***********************************************
 * @brief			: Defines
 **********************************************/

/* ----------------------------------------------------------------------------
   -- UART
   ---------------------------------------------------------------------------- */
#define _192000E1_				24 , 100 , 0
#define _UART_INIT_				LPUART_EXT_CLK	, _192000E1_  , LIN_LBKDIE_DIS , LPUART_TX_noINV , BREAK_CHAR_LEN9_LEN13 , LIN_BREAK_DETEC_DIS
#define _UART_OFF_				LPUART_NO_CLK	, _192000E1_  , LIN_LBKDIE_DIS , LPUART_TX_noINV , BREAK_CHAR_LEN9_LEN13 , LIN_BREAK_DETEC_DIS
#define _PARITY_EVEN_			LPUART_CTRL_M(1) |  LPUART_CTRL_PE(1) | LPUART_CTRL_PT(0)

/* ----------------------------------------------------------------------------
   -- RDO INTERFACE
   ---------------------------------------------------------------------------- */
#define RDO_UARTx	UART0
#define _RDO_INIT_	RDO_UARTx , _UART_INIT_
#define _RDO_OFF_	RDO_UARTx , _UART_OFF_
#define RDO_IRQ		LPUART0_RxTx_IRQn

/* ----------------------------------------------------------------------------
   -- ESP INTERFACE
   ---------------------------------------------------------------------------- */
#define ESP_UARTx		UART1
#define _ESP_INIT_		ESP_UARTx , _UART_INIT_
#define _ESP_OFF_		ESP_UARTx , _UART_OFF_
#define ESP_IRQ			LPUART1_RxTx_IRQn


#endif /* API_RDOGATEWAYPERIPHERICS_H_ */
