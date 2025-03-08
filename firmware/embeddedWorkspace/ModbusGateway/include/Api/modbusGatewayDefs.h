/*
 * ModbusGatewayDefs.h
 *
 *  Created on: 23 nov. 2024
 *      Author: toto_
 */

#ifndef API_MODBUSGATEWAYDEFS_H_
#define API_MODBUSGATEWAYDEFS_H_

#define DELTA_TX_TICKS 	( (5000) )	/**< Delta between messages */
#define TIMEOUT			4500

#define MCU_RDO_TX		&(IP_LPUART0->DATA)	/**< Embedded to write data */
#define MCU_RDO_RX		&(IP_LPUART0->DATA)	/**< Embedded to read data 	*/

#define MCU_ESP_TX		&(IP_LPUART1->DATA)	/**< Embedded to write data */
#define MCU_ESP_RX		&(IP_LPUART1->DATA)	/**< Embedded to read data 	*/

#define __NO_COMMUNICATION__	( (msec_t)(ticks-lastValidRequestFromEsp) > TIMEOUT ||  (msec_t)(ticks-lastValidRequestFromRdo) > TIMEOUT )


#endif /* API_MODBUSGATEWAYDEFS_H_ */
