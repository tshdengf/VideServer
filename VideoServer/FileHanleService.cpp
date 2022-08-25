#include "FileHanleService.h"
#include "ProcessArg.h"
#include <stdio.h>
#include "VideoServerDao.h"

void FileHanleService::Process(ProcessArg arg)
{
	INFO("Handler Client{} File Operate", arg.clientInfo.ip);

	try
	{
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

		std::string filePath = "/opt/video/vod/" + tmpFileName.substr(0, sufIndex) + "-" + std::string(upLoadInfo.md5).substr(0, 8) + suffix;

		if (access(filePath.c_str(), F_OK) == -1) return;

		//文件已经存在,并且要删除缓冲文件
		if (((access(filePath.c_str(), F_OK)) != -1) && CommonUtils::get_file_size(filePath.c_str()) != upLoadInfo.fileLen
			&&upLoadInfo.seekIndex == -1)
		{
			remove(filePath.c_str());
		}
		else if (((access(filePath.c_str(), F_OK)) != -1) && CommonUtils::get_file_size(filePath.c_str()) == upLoadInfo.fileLen
			&&upLoadInfo.seekIndex == upLoadInfo.fileLen)
		{
			//插入数据库,上传完成
			std::string sql = "insert into file (FILE_NAME, FILE_MD5, FILE_PATH) values (?,?,?)";
			ExecuteSQL(sql, upLoadInfo.fileName, upLoadInfo.md5, filePath.c_str());
		}
	}
	catch (const std::exception&ex)
	{
		INFO("Handler Client{} File Operate Error{}", arg.clientInfo.ip, ex.what());
	}

	INFO("Handler Client{} File Operate Over", arg.clientInfo.ip);
}
