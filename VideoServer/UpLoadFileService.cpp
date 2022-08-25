#include "UpLoadFileService.h"
#include "Logger.h"
#include "LibeventServer.h"
#include "VideoServerDao.h"
#include "MsgHeader.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include "CommonUtils.h"

//处理文件上传
void UpLoadFileService::Process(ProcessArg arg)
{
	INFO("Handler Client{} UpLoadFile", arg.clientInfo.ip);

	RequestReplay requestReplay;
	MsgHeader head(sizeof(requestReplay), 0);
	char sendBuffer[MSG_HEAD_LEN + sizeof(requestReplay)] = "\0";
	memcpy(sendBuffer, &head, MSG_HEAD_LEN);

	try
	{
		ServerFrame * service = (LibeventServer*)arg.argExtend;

		FileUpLoadInfo upLoadInfo;
		memcpy(&upLoadInfo, arg.content, sizeof(upLoadInfo));

		if (upLoadInfo.fileName == "\0")
		{
			INFO("{}Client UpFile Error FileName Empty", arg.clientInfo.ip);
			return;
		}
		string tmpFileName(upLoadInfo.fileName);
		int sufIndex = tmpFileName.find_last_of('.');
		std::string suffix = tmpFileName.substr(sufIndex, tmpFileName.length());

		std::string fileName = "/opt/video/vod/" + tmpFileName.substr(0, sufIndex) + "-" +  std::string(upLoadInfo.md5).substr(0,8) + suffix;

		//文件已经存在
		if (((access(fileName.c_str(), F_OK)) != -1)&& CommonUtils::get_file_size(fileName.c_str()) == upLoadInfo.fileLen)
		{
			strcpy(requestReplay.msg, "UpFile Error : File Exits");
			requestReplay.code = 0;
			memcpy(sendBuffer + MSG_HEAD_LEN, &requestReplay, sizeof(requestReplay));
			service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN + sizeof(requestReplay));
			return;
		}


		//解决自己创建的文件没有权限读写
		int fileFd = open(fileName.c_str(), O_CREAT | O_WRONLY | O_APPEND, S_IRWXU| S_IRWXG| S_IRWXO);
		if (fileFd == -1)
		{
			perror("Client UpFile:");
			INFO("{} Client UpFile Error: {}", arg.clientInfo.ip, errno);
			strcpy(requestReplay.msg, "UpFile Error : Open File Error" + errno);
			requestReplay.code = 0;
			memcpy(sendBuffer + MSG_HEAD_LEN, &requestReplay, sizeof(requestReplay));
			service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN + sizeof(requestReplay));
			return;
		}

		//每次都移动到文件末尾进行添加
		lseek(fileFd, 0, SEEK_END);

		int count = write(fileFd, upLoadInfo.buffer, upLoadInfo.bufferLen);
		//INFO("Write FileName：{} count {} ned count{}", fileName.c_str(), count, upLoadInfo.bufferLen);
		if (count == -1)
		{
			perror("Client write UpFile:");
			INFO("{} Client UpFile Error: {}", arg.clientInfo.ip, errno);
			strcpy(requestReplay.msg, "UpFile Error : Write File Error" + errno);
			requestReplay.code = 0;
			memcpy(sendBuffer + MSG_HEAD_LEN, &requestReplay, sizeof(requestReplay));
			service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN + sizeof(requestReplay));
			return;
		}

		requestReplay.code = 1;
		memcpy(sendBuffer + MSG_HEAD_LEN, &requestReplay, sizeof(requestReplay));
		service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN + sizeof(requestReplay));
		close(fileFd);
	}
	catch (const std::exception&e)
	{
		INFO("Up File Exception:{}", e.what());
	}
	
	INFO("Handler Client {} UpLoadFile End", arg.clientInfo.ip);


}
