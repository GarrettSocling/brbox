#include "SysRpc.h"
#include "ADSysInfo.hpp"
//#include "SysmgrJsonDef.h"
/* ------------------------------------------------------------------------- */
SysRpc:: SysRpc(std::string rpcName,int myIndex,bool emu,bool log,SYSMGR_CMN_DATA_CACHE *pData):ADJsonRpcMgrConsumer(rpcName,myIndex,emu,log)
{
	pDataCache=pData;	
}
/* ------------------------------------------------------------------------- */
SysRpc::~ SysRpc()
{
}
/* ------------------------------------------------------------------------- */
int SysRpc::MapJsonToBinary(JsonDataCommObj* pReq,int index)
{
	//printf("SysRpc::MapJsonToBinary called\n");
	EJSON_SYSMGR_RPC_TYPES command =(EJSON_SYSMGR_RPC_TYPES)index;
	switch(command)
	{
		case EJSON_SYSMGR_RPC_GET_LOADINFO :return json_to_bin_get_loadinfo(pReq);
		case EJSON_SYSMGR_RPC_GET_MEMINFO  :return json_to_bin_get_meminfo(pReq);
		case EJSON_SYSMGR_RPC_GET_CPUINFO  :return json_to_bin_get_cpuinfo(pReq);
		case EJSON_SYSMGR_RPC_GET_DEV_OP   :return json_to_bin_get_devop(pReq);
		case EJSON_SYSMGR_RPC_SET_DEV_OP   :return json_to_bin_set_devop(pReq);
		default:break;
	}
	return -1;//0;
}
/* ------------------------------------------------------------------------- */
int SysRpc::MapBinaryToJson(JsonDataCommObj* pReq,int index)
{
	//printf("SysRpc::MapBinaryToJson called\n");
	EJSON_SYSMGR_RPC_TYPES command =(EJSON_SYSMGR_RPC_TYPES)index;
	switch(command)
	{
		case EJSON_SYSMGR_RPC_GET_LOADINFO :return bin_to_json_get_loadinfo(pReq);
		case EJSON_SYSMGR_RPC_GET_MEMINFO  :return bin_to_json_get_meminfo(pReq);
		case EJSON_SYSMGR_RPC_GET_CPUINFO  :return bin_to_json_get_cpuinfo(pReq);
		case EJSON_SYSMGR_RPC_GET_DEV_OP   :return bin_to_json_get_devop(pReq);
		case EJSON_SYSMGR_RPC_SET_DEV_OP   :return bin_to_json_set_devop(pReq);
		default: break;
	}
	return -1;
}
/* ------------------------------------------------------------------------- */
int SysRpc::ProcessWork(JsonDataCommObj* pReq,int index,ADJsonRpcMgrProducer* pObj)
{
	//printf("SysRpc::ProcessWork called\n");
	EJSON_SYSMGR_RPC_TYPES command =(EJSON_SYSMGR_RPC_TYPES)index;
	switch(command)
	{
		case EJSON_SYSMGR_RPC_GET_LOADINFO :return process_get_loadinfo(pReq);
		case EJSON_SYSMGR_RPC_GET_MEMINFO  :return process_get_meminfo(pReq);
		case EJSON_SYSMGR_RPC_GET_CPUINFO  :return process_get_cpuinfo(pReq);
		case EJSON_SYSMGR_RPC_GET_DEV_OP   :return process_get_devop(pReq);
		case EJSON_SYSMGR_RPC_SET_DEV_OP   :return process_set_devop(pReq,pObj);
		default:break;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
RPC_SRV_RESULT SysRpc::ProcessWorkAsync(int cmd,unsigned char* pWorkData)
{
	RPC_SRV_RESULT ret_val=RPC_SRV_RESULT_FAIL;
	switch(cmd)
	{
		case EJSON_SYSMGR_RPC_SET_DEV_OP:
			{
				SYSMGR_DEV_OP_PACKET *pPacket;
				pPacket=(SYSMGR_DEV_OP_PACKET*)pWorkData;
				ret_val=process_async_set_devop(pPacket);
				OBJ_MEM_DELETE(pWorkData);
			}
			break;
		default:
			break;
	}
	return ret_val;
}
/* ------------------------------------------------------------------------- */
int SysRpc::json_to_bin_get_loadinfo(JsonDataCommObj* pReq)
{
	SYSMGR_LOAD_INFO_PACKET* pPanelCmdObj=NULL;
	PREPARE_JSON_REQUEST(RPC_SRV_REQ,SYSMGR_LOAD_INFO_PACKET,RPC_SRV_ACT_READ,EJSON_SYSMGR_RPC_GET_LOADINFO);
	return 0;
}
int SysRpc::bin_to_json_get_loadinfo(JsonDataCommObj* pReq)
{
	RPC_SRV_REQ *pPanelReq=NULL;
	SYSMGR_LOAD_INFO_PACKET* pPanelCmdObj;
	pPanelReq=(RPC_SRV_REQ *)pReq->pDataObj;
	pPanelCmdObj=(SYSMGR_LOAD_INFO_PACKET*)pPanelReq->dataRef;
	if(pPanelReq->result!=RPC_SRV_RESULT_SUCCESS)
		prepare_result_string(pPanelReq->result,pReq);
	else
	{
		prepare_result_string(pPanelReq->result,pReq,(char*)SYSMGR_RPC_LOADINFO_ARG_CURRENT,pPanelCmdObj->cur_load,
				                             (char*)SYSMGR_RPC_LOADINFO_ARG_AVERAGE,pPanelCmdObj->avg_load,
				                             (char*)SYSMGR_RPC_LOADINFO_ARG_UPTIME ,pPanelCmdObj->uptime);
	}
	OBJ_MEM_DELETE(pPanelCmdObj);
	return 0;
}
int SysRpc::process_get_loadinfo(JsonDataCommObj* pReq)
{
	ADSysInfo SysInfo;
	RPC_SRV_REQ *pPanelReq=NULL;
	pPanelReq=(RPC_SRV_REQ *)pReq->pDataObj;
	SYSMGR_LOAD_INFO_PACKET* pPacket;
	pPacket=(SYSMGR_LOAD_INFO_PACKET*)pPanelReq->dataRef;
	if(pPanelReq->action!=RPC_SRV_ACT_READ)
	{
		pPanelReq->result=RPC_SRV_RESULT_ACTION_NOT_ALLOWED;
		return 0;
	}
	if(SysInfo.read_load_info(pPacket->cur_load,pPacket->avg_load,pPacket->uptime)==0)
		pPanelReq->result=RPC_SRV_RESULT_SUCCESS;
	else
		pPanelReq->result=RPC_SRV_RESULT_FAIL;
	return 0;
}
/* ------------------------------------------------------------------------- */
int SysRpc::json_to_bin_get_meminfo(JsonDataCommObj* pReq)
{
	SYSMGR_MEM_INFO_PACKET* pPanelCmdObj=NULL;
	PREPARE_JSON_REQUEST(RPC_SRV_REQ,SYSMGR_MEM_INFO_PACKET,RPC_SRV_ACT_READ,EJSON_SYSMGR_RPC_GET_MEMINFO);
	return 0;
}
int SysRpc::bin_to_json_get_meminfo(JsonDataCommObj* pReq)
{
	RPC_SRV_REQ *pPanelReq=NULL;
	SYSMGR_MEM_INFO_PACKET* pPanelCmdObj;
	pPanelReq=(RPC_SRV_REQ *)pReq->pDataObj;
	pPanelCmdObj=(SYSMGR_MEM_INFO_PACKET*)pPanelReq->dataRef;
	if(pPanelReq->result!=RPC_SRV_RESULT_SUCCESS)
		prepare_result_string(pPanelReq->result,pReq);
	else
	{
		prepare_result_string(pPanelReq->result,pReq,(char*)SYSMGR_RPC_MEMINFO_ARG_MEM,pPanelCmdObj->mem,
				                             (char*)SYSMGR_RPC_MEMINFO_ARG_MEMFREE,pPanelCmdObj->memfree,
				                             (char*)SYSMGR_RPC_MEMINFO_ARG_MEMUSED,pPanelCmdObj->memused);
	}
	OBJ_MEM_DELETE(pPanelCmdObj);
	return 0;
}
int SysRpc::process_get_meminfo(JsonDataCommObj* pReq)
{
	ADSysInfo SysInfo;
	RPC_SRV_REQ *pPanelReq=NULL;
	pPanelReq=(RPC_SRV_REQ *)pReq->pDataObj;
	SYSMGR_MEM_INFO_PACKET* pPacket;
	pPacket=(SYSMGR_MEM_INFO_PACKET*)pPanelReq->dataRef;
	if(pPanelReq->action!=RPC_SRV_ACT_READ)
	{
		pPanelReq->result=RPC_SRV_RESULT_ACTION_NOT_ALLOWED;
		return 0;
	}
	if(SysInfo.read_mem_info(pPacket->mem,pPacket->memfree,pPacket->memused)==0)
		pPanelReq->result=RPC_SRV_RESULT_SUCCESS;
	else
		pPanelReq->result=RPC_SRV_RESULT_FAIL;
	return 0;
}
/* ------------------------------------------------------------------------- */
int SysRpc::json_to_bin_get_cpuinfo(JsonDataCommObj* pReq)
{
	SYSMGR_CPU_INFO_PACKET* pPanelCmdObj=NULL;
	PREPARE_JSON_REQUEST(RPC_SRV_REQ,SYSMGR_CPU_INFO_PACKET,RPC_SRV_ACT_READ,EJSON_SYSMGR_RPC_GET_CPUINFO);
	return 0;
}
int SysRpc::bin_to_json_get_cpuinfo(JsonDataCommObj* pReq)
{
	RPC_SRV_REQ *pPanelReq=NULL;
	SYSMGR_CPU_INFO_PACKET* pPanelCmdObj;
	pPanelReq=(RPC_SRV_REQ *)pReq->pDataObj;
	pPanelCmdObj=(SYSMGR_CPU_INFO_PACKET*)pPanelReq->dataRef;
	if(pPanelReq->result!=RPC_SRV_RESULT_SUCCESS)
		prepare_result_string(pPanelReq->result,pReq);
	else
	{
		prepare_result_string(pPanelReq->result,pReq,(char*)SYSMGR_RPC_CPUINFO_ARG_MODEL,pPanelCmdObj->cpu_model,
				                             (char*)SYSMGR_RPC_CPUINFO_ARG_CORES,pPanelCmdObj->cpu_cores,
				                             (char*)SYSMGR_RPC_CPUINFO_ARG_FREQ ,pPanelCmdObj->cpu_freq);
	}
	OBJ_MEM_DELETE(pPanelCmdObj);
	return 0;
}
int SysRpc::process_get_cpuinfo(JsonDataCommObj* pReq)
{
	ADSysInfo SysInfo;
	RPC_SRV_REQ *pPanelReq=NULL;
	pPanelReq=(RPC_SRV_REQ *)pReq->pDataObj;
	SYSMGR_CPU_INFO_PACKET* pPacket;
	pPacket=(SYSMGR_CPU_INFO_PACKET*)pPanelReq->dataRef;
	if(pPanelReq->action!=RPC_SRV_ACT_READ)
	{
		pPanelReq->result=RPC_SRV_RESULT_ACTION_NOT_ALLOWED;
		return 0;
	}

	if(SysInfo.read_cpu_info(pPacket->cpu_model,pPacket->cpu_cores,pPacket->cpu_freq)==0)
		pPanelReq->result=RPC_SRV_RESULT_SUCCESS;
	else
		pPanelReq->result=RPC_SRV_RESULT_FAIL;
	return 0;
}
/* ------------------------------------------------------------------------- */
int SysRpc::json_to_bin_get_devop(JsonDataCommObj* pReq)
{
	SYSMGR_DEV_OP_PACKET* pPanelCmdObj=NULL;
	PREPARE_JSON_REQUEST(RPC_SRV_REQ,SYSMGR_DEV_OP_PACKET,RPC_SRV_ACT_READ,EJSON_SYSMGR_RPC_GET_DEV_OP);
	return 0;
}
int SysRpc::bin_to_json_get_devop(JsonDataCommObj* pReq)
{
	PREPARE_JSON_RESP_ENUM(RPC_SRV_REQ,SYSMGR_DEV_OP_PACKET,SYSMGR_RPC_DEV_OP_ARG,operation,SYSMGR_RPC_DEV_OP_ARG_TABL,EJSON_SYSMGR_DEV_OP_UNKNOWN);
	return 0;
}
int SysRpc::process_get_devop(JsonDataCommObj* pReq)
{
	ADSysInfo SysInfo;
	RPC_SRV_REQ *pPanelReq=NULL;
	pPanelReq=(RPC_SRV_REQ *)pReq->pDataObj;
	SYSMGR_DEV_OP_PACKET* pPacket;
	pPacket=(SYSMGR_DEV_OP_PACKET*)pPanelReq->dataRef;
	if(pPanelReq->action!=RPC_SRV_ACT_READ)
	{
		pPanelReq->result=RPC_SRV_RESULT_ACTION_NOT_ALLOWED;
		return 0;
	}
	//pDataCache->DevOp.operation=EJSON_SYSMGR_DEV_OP_ON;//TODO: init this state correctly on startup

	pPacket->operation=pDataCache->DevOp.operation;
	pPanelReq->result=RPC_SRV_RESULT_SUCCESS;
	return 0;
}
int SysRpc::json_to_bin_set_devop(JsonDataCommObj* pReq)
{
	SYSMGR_DEV_OP_PACKET* pPanelCmdObj=NULL;
	PREPARE_JSON_REQUEST(RPC_SRV_REQ,SYSMGR_DEV_OP_PACKET,RPC_SRV_ACT_WRITE,EJSON_SYSMGR_RPC_SET_DEV_OP);
	JSON_STRING_TO_ENUM(SYSMGR_RPC_DEV_OP_ARG,SYSMGR_RPC_DEV_OP_ARG_TABL,EJSON_SYSMGR_DEV_OP,EJSON_SYSMGR_DEV_OP_UNKNOWN,pPanelCmdObj->operation);
	return 0;
}
int SysRpc::bin_to_json_set_devop(JsonDataCommObj* pReq)
{
	PREPARE_JSON_RESP_IN_PROG(RPC_SRV_REQ,SYSMGR_DEV_OP_PACKET,RPCMGR_RPC_TASK_STS_ARGID);
	return 0;
}
int SysRpc::process_set_devop(JsonDataCommObj* pReq,ADJsonRpcMgrProducer* pObj)
{
	RPC_SRV_REQ *pPanelReq=NULL;
	pPanelReq=(RPC_SRV_REQ *)pReq->pDataObj;
	SYSMGR_DEV_OP_PACKET* pPacket;
	pPacket=(SYSMGR_DEV_OP_PACKET*)pPanelReq->dataRef;
	if(pPanelReq->action!=RPC_SRV_ACT_WRITE)
	{
		pPanelReq->result=RPC_SRV_RESULT_ACTION_NOT_ALLOWED;
		return 0;
	}

	SYSMGR_DEV_OP_PACKET* pWorkData=NULL;
	OBJECT_MEM_NEW(pWorkData,SYSMGR_DEV_OP_PACKET);//delete this object in run_work() callback function
	if(pWorkData == NULL)
	{
		pPanelReq->result=RPC_SRV_RESULT_MEM_ERROR;
		return -1;
	}
	pWorkData->operation=pPacket->operation;
	pPanelReq->result=pObj->PushAsyncTask(EJSON_SYSMGR_RPC_SET_DEV_OP,(unsigned char*)pWorkData,&pPacket->taskID,WORK_CMD_AFTER_DONE_PRESERVE);
	if(pPanelReq->result!=RPC_SRV_RESULT_IN_PROG)
		OBJ_MEM_DELETE(pWorkData);
	return 0;
}
RPC_SRV_RESULT SysRpc::process_async_set_devop(SYSMGR_DEV_OP_PACKET* pPacket)
{
	char cmdline[512];
	ADSysInfo SysInfo;
	RPC_SRV_RESULT ret_val=RPC_SRV_RESULT_ARG_ERROR;
	switch(pPacket->operation)
	{
		case EJSON_SYSMGR_DEV_OP_IDLE   :
		case EJSON_SYSMGR_DEV_OP_ON     :
		case EJSON_SYSMGR_DEV_OP_STANDBY:
			pDataCache->DevOp.operation=pPacket->operation;
			ret_val=RPC_SRV_RESULT_SUCCESS;
			break;
		//case EJSON_SYSMGR_DEV_OP_BOOT :
		case EJSON_SYSMGR_DEV_OP_REBOOT :
			sprintf(cmdline,"%s","sleep 3;reboot");//CMDLINE_TRIGGER_REBOOT_CMD);
			//#define CMDLINE_TRIGGER_REBOOT_CMD       "sleep 3;reboot"
			ret_val=SysInfo.run_shell_script(cmdline,get_emulation_flag());
		default:
			break;
	}
	return ret_val;
}
/* ------------------------------------------------------------------------- */

