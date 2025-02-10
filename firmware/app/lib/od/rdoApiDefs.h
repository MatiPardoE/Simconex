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

// RDO GPIOS
#define RDO_DE_RE_GPIO GPIO_NUM_27
#define RDO_RX_GPIO GPIO_NUM_35
#define RDO_TX_GPIO GPIO_NUM_32

// RDO UART
#define RDO_BAUD_RATE 19200

//
#define _TIMEOUT_RDO_INIT_ 1000
#define _TIMEOUT_RDO_REQUEST_ 1000
// #define _TIMEOUT_RDO_REQUEST_     30000
// #define _TIMEOUT_RDO_REQUEST_     10000

#define RDO_SLAVE_ID 0x01

// TEST GPIO
#define RDO_LED1 GPIO_NUM_5
#define RDO_LED2 GPIO_NUM_18
#define RDO_LED3 GPIO_NUM_19
#define RDO_LED4 GPIO_NUM_21

//
#define DATA_BYTE_ID 5
#define SENSOR_CMD_REG 0x2458
#define CAL_NUM_REGS 0x01

#define EPSILON_MEASURE 0.1
#define EQUILIBRIUM_ACHIEVED 2

#define MAX_ERRORS      30

/***********************************************
 * @brief			: Enum
 **********************************************/
typedef enum
{
    // Header Registers
    _SENSOR_ID_ = 40001 - 40001,
    _SERIAL_NUMBER_ = 40002 - 40001,
    _SENSOR_STATUS_ = 40004 - 40001,
    _LAST_FACTORY_CAL_ = 40005 - 40001,
    _NEXT_FACTORY_CAL_ = 40008 - 40001,
    _LAST_USER_CAL_ = 40011 - 40001,
    _NEXT_USER_CAL_ = 40014 - 40001,
    _WARM_UP_TIME_ = 40017 - 40001,
    _FAST_SAMPLE_RATE_ = 40018 - 40001,
    _NUMBER_SENSORS_PARAM_ = 40019 - 40001,
    _ALARM_PARAM_NUMBER_ = 40020 - 40001,
    _ALARM_ENABLE_BITS_ = 40021 - 40001,
    _HIGH_ALARM_SET_ = 40022 - 40001,
    _HIGH_ALARM_CLEAR_ = 40024 - 40001,
    _HIGH_WARNING_SET_ = 40026 - 40001,
    _HIGH_WARNING_CLEAR_ = 40028 - 40001,
    _LOW_WARNING_SET_ = 40030 - 40001,
    _LOW_WARNING_CLEAR_ = 40032 - 40001,
    _LOW_ALARM_SET_ = 40034 - 40001,
    _LOW_ALARM_CLEAR_ = 40036 - 40001,

    // DO Concentration Parameters
    _MEASURED_VALUE_DO_ = 40038 - 40001,
    _PARAMETER_ID_DO_ = 40040 - 40001,
    _UNIT_ID_DO_ = 40041 - 40001,
    _DATA_QUALITY_ID_DO_ = 40042 - 40001,
    _SENTINEL_VALUE_DO_ = 40043 - 40001,
    _AVAILABLE_UNITS_DO_ = 40045 - 40001,

    // Temperature Parameters
    _MEASURED_VALUE_TEMP_ = 40046 - 40001,
    _PARAMETER_ID_TEMP_ = 40048 - 40001,
    _UNIT_ID_TEMP_ = 40049 - 40001,
    _DATA_QUALITY_ID_TEMP_ = 40050 - 40001,
    _SENTINEL_VALUE_TEMP_ = 40051 - 40001,
    _AVAILABLE_UNITS_TEMP_ = 40053 - 40001,

    // DO Saturation
    _MEASURED_VALUE_DO_SAT_ = 40054 - 40001,
    _PARAMETER_ID_DO_SAT_ = 40056 - 40001,
    _UNIT_ID_DO_SAT_ = 40057 - 40001,
    _DATA_QUALITY_ID_DO_SAT_ = 40058 - 40001,
    _SENTINEL_VALUE_DO_SAT_ = 40059 - 40001,
    _AVAILABLE_UNITS_DO_SAT_ = 40061 - 40001,

    // Oxygen Partial Pressure
    _MEASURED_VALUE_OXY_PP_ = 40062 - 40001,
    _PARAMETER_ID_OXY_PP_ = 40064 - 40001,
    _UNIT_ID_OXY_PP_ = 40065 - 40001,
    _DATA_QUALITY_ID_OXY_PP_ = 40066 - 40001,
    _SENTINEL_VALUE_OXY_PP_ = 40067 - 40001,
    _AVAILABLE_UNITS_OXY_PP_ = 40069 - 40001,

    // External Voltage
    _MEASURED_VALUE_EXTV_ = 40070 - 40001,
    _PARAMETER_ID_EXTV_ = 40072 - 40001,
    _UNIT_ID_EXTV_ = 40073 - 40001,
    _DATA_QUALITY_ID_EXTV_ = 40074 - 40001,
    _SENTINEL_VALUE_EXTV_ = 40075 - 40001,
    _AVAILABLE_UNITS_EXTV_ = 40076 - 40001,

    // Calibration Registers
    _LIVE_SALINITY_VALUE_ = 40118 - 40001,
    _DEFAULT_SALINITY_VALUE_ = 40120 - 40001,
    _LIVE_BAROMETRIC_PRESS_ = 40122 - 40001,

    _DEFAULT_BAROMETRIC_PRESS_ = 40124 - 40001,
    _SATURATION_CONC_100_ = 40126 - 40001,

    _SATURATION_TEMP_100_ = 40128 - 40001, // 7F
    _SATURATION_SALINITY_100_ = 40130 - 40001,
    _SATURATION_BAR_PRESS_100_ = 40132 - 40001,

    _SATURATION_CONC_0_ = 40134 - 40001,
    _SATURATION_TEMP_0_ = 40136 - 40001,
    _CALIBRATION_SLOPE_ = 40138 - 40001,
    _CALIBRATION_OFFSET_ = 40140 - 40001,
    _CALIBRATION_STATUS_ = 40142 - 40001
} rdoRegisters_t;

typedef enum
{
    // Header Registers
    _SENSOR_ID_SIZE_ = 1,
    _SERIAL_NUMBER_SIZE_ = 2,
    _SENSOR_STATUS_SIZE_ = 1,
    _LAST_FACTORY_CAL_SIZE_ = 1,
    _NEXT_FACTORY_CAL_SIZE_ = 3, // Corrección
    _LAST_USER_CAL_SIZE_ = 3,    // Corrección
    _NEXT_USER_CAL_SIZE_ = 3,    // Corrección
    _WARM_UP_TIME_SIZE_ = 1,
    _FAST_SAMPLE_RATE_SIZE_ = 1,
    _NUMBER_SENSORS_PARAM_SIZE_ = 1,
    _ALARM_PARAM_NUMBER_SIZE_ = 1,
    _ALARM_ENABLE_BITS_SIZE_ = 1,
    _HIGH_ALARM_SET_SIZE_ = 2,
    _HIGH_ALARM_CLEAR_SIZE_ = 2,
    _HIGH_WARNING_SET_SIZE_ = 2,
    _HIGH_WARNING_CLEAR_SIZE_ = 2,
    _LOW_WARNING_SET_SIZE_ = 2,
    _LOW_WARNING_CLEAR_SIZE_ = 2,
    _LOW_ALARM_SET_SIZE_ = 2,
    _LOW_ALARM_CLEAR_SIZE_ = 2,

    // DO Concentration Parameters
    //_MEASURED_VALUE_DO_SIZE_      = 2,
    _MEASURED_VALUE_DO_SIZE_ = 8,
    _PARAMETER_ID_DO_SIZE_ = 1,
    _UNIT_ID_DO_SIZE_ = 1,
    _DATA_QUALITY_ID_DO_SIZE_ = 1,
    _SENTINEL_VALUE_DO_SIZE_ = 2,
    _AVAILABLE_UNITS_DO_SIZE_ = 1,

    // Temperature Parameters
    _MEASURED_VALUE_TEMP_SIZE_ = 8,
    _PARAMETER_ID_TEMP_SIZE_ = 1,
    _UNIT_ID_TEMP_SIZE_ = 1,
    _DATA_QUALITY_ID_TEMP_SIZE_ = 1,
    _SENTINEL_VALUE_TEMP_SIZE_ = 2,
    _AVAILABLE_UNITS_TEMP_SIZE_ = 1,

    // DO Saturation
    _MEASURED_VALUE_DO_SAT_SIZE_ = 8,
    _PARAMETER_ID_DO_SAT_SIZE_ = 1,
    _UNIT_ID_DO_SAT_SIZE_ = 1,
    _DATA_QUALITY_ID_DO_SAT_SIZE_ = 1,
    _SENTINEL_VALUE_DO_SAT_SIZE_ = 2,
    _AVAILABLE_UNITS_DO_SAT_SIZE_ = 1,

    // Oxygen Partial Pressure
    _MEASURED_VALUE_OXY_PP_SIZE_ = 2,
    _PARAMETER_ID_OXY_PP_SIZE_ = 1,
    _UNIT_ID_OXY_PP_SIZE_ = 1,
    _DATA_QUALITY_ID_OXY_PP_SIZE_ = 1,
    _SENTINEL_VALUE_OXY_PP_SIZE_ = 2,
    _AVAILABLE_UNITS_OXY_PP_SIZE_ = 1,

    // External Voltage
    _MEASURED_VALUE_EXTV_SIZE_ = 2,
    _PARAMETER_ID_EXTV_SIZE_ = 1,
    _UNIT_ID_EXTV_SIZE_ = 1,
    _DATA_QUALITY_ID_EXTV_SIZE_ = 1,
    _SENTINEL_VALUE_EXTV_SIZE_ = 2,
    _AVAILABLE_UNITS_EXTV_SIZE_ = 1,

    // External Voltage
    _LIVE_SALINITY_VALUE_SIZE_ = 2,
    _DEFAULT_SALINITY_VALUE_SIZE_ = 2,
    _LIVE_BAROMETRIC_PRESS_SIZE_ = 2,
    _DEFAULT_BAROMETRIC_PRESS_SIZE_ = 2,
    _SATURATION_CONC_100_SIZE_ = 2,
    _SATURATION_TEMP_100_SIZE_ = 2,
    _SATURATION_SALINITY_100_SIZE_ = 2,
    _SATURATION_BAR_PRESS_100_SIZE_ = 2,
    _SATURATION_CONC_0_SIZE_ = 2,
    _SATURATION_TEMP_0_SIZE_ = 2,
    _CALIBRATION_SLOPE_SIZE_ = 2,
    _CALIBRATION_OFFSET_SIZE_ = 2,
    _CALIBRATION_STATUS_SIZE_ = 1
} rdoRegistersSz_t;

typedef enum
{
    // header
    SENSOR_ID = 1,
    SERIAL_NUMBER,
    SENSOR_STATUS,
    LAST_FACTORY_CALIBRATION,
    NEXT_FACTORY_CALIBRATION,
    LAST_USER_CALIBRATION,
    NEXT_USER_CALIBRATION,
    GET_DO,
    GET_TEMP,
    GET_DO_SAT,

    WRITE_CALIBRATION_COMMAND,

    SET_LIVE_BAR_PRESSURE,
    SET_100_SAT_BAR_PRESSURE,
    SET_100_SAT_SALINITY,
    SET_100_SAT_CONCENTRATION,
    SET_100_SAT_TEMPERATURE,

    //NUEVO
    SET_0_SAT_CONCENTRATION,
    SET_0_SAT_TEMPERATURE,

    UPDATE_CALIBRATION_COMMAND,

    WRITE_CALIBRATION_COMMAND_OFF,

} rdoStatus_t;

typedef enum
{
    DO_CONCENTRATION_ID = 20,
    TEMPERATURE_ID = 1,
    DO_SATURATION_ID = 21,
} rdoParameterId_t;

typedef enum{
    CALIB_SAT_1P = 0,
    CALIB_SAT_2P    ,
} rdoCalibSat_t;

/***********************************************
 * @brief			: Macros
 **********************************************/
#define _TIMEOUT_TO_RDO_INIT_ ((uint32_t)(millis() - lastMillisRDO) > (uint32_t)(_TIMEOUT_RDO_INIT_))
#define _TIMEOUT_TO_RDO_REQUEST_ ((uint32_t)(millis() - lastMillisRDO) > (uint32_t)(_TIMEOUT_RDO_REQUEST_))
#define _updateTimeout_ lastMillisRDO = millis()
#define _NOT_CALIBRATING_CMD_ (rdo.status != WRITE_CALIBRATION_COMMAND && rdo.status != UPDATE_CALIBRATION_COMMAND && rdo.status != WRITE_CALIBRATION_COMMAND_OFF && rdo.status != SET_LIVE_BAR_PRESSURE && rdo.status != SET_100_SAT_BAR_PRESSURE && rdo.status != SET_100_SAT_SALINITY && rdo.status != SET_100_SAT_CONCENTRATION && rdo.status != SET_100_SAT_TEMPERATURE)

#define _HAS_MORE_ERRORS_THAN_(x) rdo.errors > x

#endif