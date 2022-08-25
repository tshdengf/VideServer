#include "HeartBeatService.h"
#include "ProcessArg.h"
#include "Logger.h"
#include "ServerFrame.h"
#include "LibeventServer.h"

void HeartBeatService::Process(ProcessArg arg)
{
	INFO("receive client:{} ip:{} port:{} heartBeat", arg.sockfd, arg.clientInfo.ip, arg.clientInfo.port);
	ServerFrame * service = (LibeventServer*)arg.argExtend;
	service->HandlerHart(arg.sockfd);
	INFO("client:{} ip:{} port:{} heartCheck Down", arg.sockfd, arg.clientInfo.ip, arg.clientInfo.port);
}
