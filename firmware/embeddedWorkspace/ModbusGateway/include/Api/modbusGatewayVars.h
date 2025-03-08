/*
 * ModbusGatewayVars.h
 *
 *  Created on: 23 nov. 2024
 *      Author: toto_
 */

#ifndef API_MODBUSGATEWAYVARS_H_
#define API_MODBUSGATEWAYVARS_H_

/***********************************************
 * @brief			: vars
 **********************************************/
extern __RW msec_t		ticks;
extern msec_t lastValidRequestFromEsp;
extern msec_t lastValidRequestFromRdo;

extern __RW MODBUS_t 	sensorRdo;
extern __RW gpio_t 		ctsRdo;
extern __RW gpio_t 		rtsRdo;

extern __RW MODBUS_t 	esp;

#endif /* API_MODBUSGATEWAYVARS_H_ */
