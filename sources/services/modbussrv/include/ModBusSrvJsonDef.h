#ifndef __MODBUSSRV_JSON_DEF_H_
#define __MODBUSSRV_JSON_DEF_H_
/* ------------------------------------------------------------------------- */
#include "ADCmnPortList.h"
#include "ADCommon.hpp"
//#include "DisplayDevice.hpp"
#define MODBUSSRV_JSON_PORT_NUMBER    ADCMN_PORT_MODBUSSRV
/* ------------------------------------------------------------------------- */
typedef enum EJSON_MODBUSSRV_RPC_TYPES_T
{
	EJSON_MODBUSSRV_RPC_MODBUS_INIT,
	EJSON_MODBUSSRV_RPC_VOLTAGE_GET, //read voltage from energy meter

	EJSON_MODBUSSRV_RPC_END,
	EJSON_MODBUSSRV_RPC_NONE
}EJSON_MODBUSSRV_RPC_TYPES;
/* ------------------------------------------------------------------------- */
//EJSON_MODBUSSRV_RPC_VOLTAGE_GET
#define MODBUSSRV_RPC_VOLTAGE_GET  "modbus_get_voltage"
#define MODBUSSRV_RPC_VOLTAGE_ARG  "voltage"
typedef struct MODBUSSRV_VOLTAGE_ACCESS_PACKET_T
{
	int voltage;
}MODBUSSRV_VOLTAGE_ACCESS_PACKET;
/* ------------------------------------------------------------------------- */
//keep all the data related to modbus-server here
typedef struct MODBUSSRV_CMN_DATA_CACHE_T
{
	void *pDevInfo;//device-info-struct(typecast in rpc handlers)
	void *pEventNotifier;//event notifier object
	void *pDevAccess;//modbus device access node

	int voltage;
	//DisplayDevice *pDisplay;
	MODBUSSRV_CMN_DATA_CACHE_T() //constructor(initializer)
	{
		pDevAccess=NULL;
		pDevInfo=NULL;
		pEventNotifier=NULL;
	};
}MODBUSSRV_CMN_DATA_CACHE;
/* ------------------------------------------------------------------------- */
typedef enum MODBUSSRV_EVENT_TYPE_T
{
	MODBUSSRV_EVENT_TYPE_OUTPUT_CHANGED=ADLIB_EVENT_NUM_END,
	MODBUSSRV_EVENT_TYPE_END,
	MODBUSSRV_EVENT_TYPE_NONE
}MODBUSSRV_EVENT_TYPE;
#define SERVER_JSON_PORT_NUM MODBUSSRV_JSON_PORT_NUMBER
#endif

