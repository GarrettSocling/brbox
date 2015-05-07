#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ADJsonRpcMgr.hpp"
#include "MyCmdline.h"
#include "SrcControlVersion.h"
#include "ADTimer.hpp"

//gpioctl
#include "GpioctlJsonDef.h"
#include "GpioCtrlRpc.h"

using namespace std;
int main(int argc, const char* argv[])
{
	//cmdline parsing
	char ver[255];snprintf(ver,254,"%05d",SRC_CONTROL_VERSION);
	MyCmdline CmdLine(CMDLINE_HELPER_MODE_SERVER,GPIOCTL_JSON_PORT_NUMBER,ver);//40000 is the portnumber
	CmdLine.parse_cmdline_arguments(argc,(char**)argv);
	if(CmdLine.is_help_printed())
		return -1;//user just requested to print the help info

	bool dbglog = CmdLine.get_debug_log();
	bool emulat = CmdLine.get_emulation_mode();

	//start 100ms timer
	ADTimer AppTimer(100);//only one instance per application(or process) must exist

	//create a common data Cache of the service
	GPIOCTL_CMN_DATA_CACHE DataCache;

	//attach rpc classes to ADJsonRpcMgr
	ADJsonRpcMgr RpcMgr(SRC_CONTROL_VERSION,dbglog); //main rpc handler

	//SmarteyeRpc PatternGet(SMARTEYE_RPC_ID_PATTERN_CHECK ,EJSON_SMARTEYE_RPC_CHECK_ID_PATTERN ,emulat,dbglog,&DataCache);
	//SmarteyeRpc DbgFileGet(SMARTEYE_RPC_DEBUG_OUTFILE_GET,EJSON_SMARTEYE_RPC_DEBUG_OUTFILE_GET,emulat,dbglog,&DataCache);
	//SmarteyeRpc DbgFileSet(SMARTEYE_RPC_DEBUG_OUTFILE_SET,EJSON_SMARTEYE_RPC_DEBUG_OUTFILE_SET,emulat,dbglog,&DataCache);

	//RpcMgr.AttachRpc(&PatternGet);
	//RpcMgr.AttachRpc(&DbgFileGet);
	//RpcMgr.AttachRpc(&DbgFileSet);

	//start listening for rpc-commands
	RpcMgr.AttachHeartBeat(&AppTimer);//attach 100ms heartbeat to ADJsonRpcMgr
	//RpcMgr.SupportShutdownRpc(false);//this is a system-manager, needs to be alive all the time, hence dont support shutdown via rpc
	RpcMgr.Start(CmdLine.get_port_number(),CmdLine.get_socket_log(),CmdLine.get_emulation_mode());
	//server is ready to serve rpc's
	RpcMgr.SetServiceReadyFlag(EJSON_RPCGMGR_READY_STATE_READY);


	//wait for sigkill or sigterm signal
	AppTimer.wait_for_exit_signal();//loop till KILL or TERM signal is received
	AppTimer.stop_timer();//stop sending heart-beats to other objects
	return 0;
}