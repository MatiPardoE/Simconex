/**
 ******************************************************************************
 * @file           	: rdoApiDefs.h
 * @author         	: Tobias Bavasso Piizzi
 * @brief			: 
 * @date			: 2024/08/13
 * @version			: 1.00
 ******************************************************************************
 * @attention		: This file it's NOT dependent from
 * 					  uc's family used
 * 					  Este archivo contiene todas las etiquetas del proyecto
 *
 ******************************************************************************
 */

#ifndef _RDO_API_DEFS_H
#define _RDO_API_DEFS_H

#include <Arduino.h>
#include <driver/uart.h>

/***********************************************
 * @brief			: Defines
 **********************************************/
#define __RW volatile

//RDO GPIOS
#define RDO_DE_RE_GPIO  GPIO_NUM_27
#define RDO_RX_GPIO     GPIO_NUM_35
#define RDO_TX_GPIO     GPIO_NUM_32
//                          TX              RX          RTS = no usar       CTS = no usar              
#define _RDO_GPIOS_     RDO_TX_GPIO , RDO_RX_GPIO , UART_PIN_NO_CHANGE , UART_PIN_NO_CHANGE

//RDO UART
#define RDO_UART_NUM        UART_NUM_2
#define RDO_BAUD_RATE       19200
#define RDO_DATA_BITS       UART_DATA_8_BITS
#define RDO_PARITY          UART_PARITY_EVEN
#define RDO_STOP_BITS       UART_STOP_BITS_1
#define RDO_FLOW_CONTROL    UART_HW_FLOWCTRL_DISABLE
#define RDO_FLOW_CONTROL_THR    122 //entiendo que es dummy

#define RDO_BUFFER_SIZE     512



/***********************************************
 * @brief			: Enum
 **********************************************/


/***********************************************
 * @brief			: Macros
 **********************************************/

#endif