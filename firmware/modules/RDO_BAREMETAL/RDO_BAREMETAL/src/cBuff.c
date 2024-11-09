/**
 ******************************************************************************
 * @file      : cBuff.c
 * @author    : Tobias Bavasso Piizzi
 * @brief			: 
 * @date			: 2023/12/24
 * @version		: 1.00
 ******************************************************************************
 * @attention		: This file it's dependent from
 * 					      uc's family used
 *
 ******************************************************************************
 */



/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <Arduino.h>
#include <rdoApi.h>

#include <rdoApiGlobalVariables.h>
#include <cBuff.h>

/***********************************************
 * @brief			: Function's  CODE
 **********************************************/

/***********************************************************************************
  * @function
  * @brief
  * @param  	None
  * @retval 	None
  **********************************************************************************/
/***********************************************************************************
  * @function 	setCircBuff
  * @brief  	Init a variable from type circ_buff_t
  * @details	Buffer provided is cleaned.
  * @param  	buff 	: Adrress where values will be stores
  * @param  	len		: last item to store
  * @retval 	Circular Buffer's struct filled
  **********************************************************************************/
__RW circ_buff_t newCircBuff	( __RW uint8_t	*buff ,	uint16_t len ){
	circ_buff_t cb = {.buff=ptrNULL };
	uint16_t i = 0;

	if(buff == NULL)	return(cb);

	//could be memset
	while(i<len){		//clean the buffer provided
		buff[i] = 0;
		i++;
	}

	cb.full		= CB_EMPTY;
	cb.inx_in	= 0;
	cb.inx_out	= 0;

	cb.buff		= buff;
	cb.len		= len;

	return(cb);
}

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