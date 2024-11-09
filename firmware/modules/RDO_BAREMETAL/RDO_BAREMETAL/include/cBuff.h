/**
 ******************************************************************************
 * @file           	: cBuff.h
 * @author         	: Tobias Bavasso Piizzi
 * @brief			: 
 * @date			: 2023/12/24
 * @version			: 1.00
 ******************************************************************************
 * @attention		: This file it's NOT dependent from
 * 					  uc's family used
 *
 ******************************************************************************
 */

#ifndef _CIRCULAR_BUFFER_MBED_H
#define _CIRCULAR_BUFFER_MBED_H


/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
//#include <std/std.h>
#include <rdoApiDefs.h>

/***********************************************
 * @brief			: Defines
 **********************************************/


/***********************************************
 * @brief			: Enum
 **********************************************/
typedef  enum{
	CB_NOT_FULL 	= 0x00,
	CB_FULL 		= 0x01,
	CB_EMPTY 		= 0x02
}	CIRC_BUFF_FULL;

/***********************************************
 * @brief			: Macros
 **********************************************/


/***********************************************
 * @brief			: Structs declaration
 **********************************************/

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
/***********************************************************************************
* @struct circ_buff_t
* @brief  circular buffer
 **********************************************************************************/
#ifdef _PRAGMA_SUPPORT_
#pragma pack(push)
#pragma pack(1)		
#endif
typedef struct {

	/*
	 * 	-------------------------------------------------------------------------
	 * |	0	|	1	|	...	|	...	|	...	|		|		|	126	|	127	|
	 * 	------------------------------------------------------------------------
	 * 				in						out
	 *
	 *
	 * */

	/**
	* @name Status & Control
	*/
	/*@{*/
	uint8_t 	full;				/**< Buffer's status*/
	uint16_t	inx_in;				/**< Input index */
	uint16_t	inx_out;			/**< Output index*/
	/*@}*/

	/**
	* @name Useful
	*/
	uint16_t		len;				/**< Max position in buffer*/
	__RW uint8_t	*buff;				/**< Pointer to buffer*/
	/*@}*/

} circ_buff_t;
#ifdef _PRAGMA_SUPPORT_
#pragma pack(pop)
#endif


/***********************************************
 * @brief			: Function prototype
 **********************************************/

/*RET	NAME			argN							*/
extern __RW circ_buff_t newCircBuff		( __RW uint8_t	*buff ,	uint16_t len	);

extern int8_t 	pushCircBuff		( __RW circ_buff_t *cb	, 	uint8_t src				);
extern uint32_t popCircBuff			( uint8_t *dst			, 	__RW circ_buff_t *cb	);

/***********************************************
 * @brief			: Variables declaration
 **********************************************/





#endif
