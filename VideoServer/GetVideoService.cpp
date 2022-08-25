#include "GetVideoService.h"
#include "Logger.h"
#include "VideoServerDao.h"
#include "LibeventServer.h"
#include "MsgHeader.h"
#include <string>
#include <unordered_map>
#include <list>
#include <vector>


#include <nlohmann/json.hpp>

using json = nlohmann::json;

//请求视频
void GetVideoService::Process(ProcessArg arg)
{
	INFO("Handler Client{} GetVideoService", arg.clientInfo.ip);

	try
	{
		MsgHeader header;
		ServerFrame * service = (LibeventServer*)arg.argExtend;
		char sendBuffer[MSG_HEAD_LEN] = "\0";

		std::string sql = "select * from file";
		list<unordered_map<string, string>> fileRes = ExecuteQueryNoParam(sql);
		if (fileRes.size() == 0)
		{
			header.contentLength = 0;
			memcpy(sendBuffer, &header, MSG_HEAD_LEN);
			//发送消息没有视频
			service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN);
			return;
		}
		INFO("Video Count：{}", fileRes.size());
		//vector<VideoFileStruct> sendVideoVec(fileRes.size());
		json jsonArray = json::array();
		
		for (auto it = fileRes.begin(); it != fileRes.end(); it++)
		{
			auto mapIt1 = (*it).find("FILE_PATH");
			auto mapIt2 = (*it).find("FILE_NAME");
			if (mapIt1 != (*it).end() && mapIt2 != (*it).end())
			{
				json jobj = json::object();
				jobj["FILE_NAME"] = mapIt2->second;
				jobj["FILE_PATH"] = mapIt1->second;
				jsonArray.push_back(jobj);
			}
		}

		string contentJson = jsonArray.dump();
		INFO("Send Video Buffer：{}", contentJson.c_str());

		int contentLen = contentJson.size();
		header.contentLength = contentLen;
		memcpy(sendBuffer, &header, MSG_HEAD_LEN);
		service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN);

		service->Send(arg.bev, contentJson.c_str(), contentLen);

	}
	catch (const std::exception& ex)
	{
		INFO("Handler Client{} GetVideoService Exception{}", arg.clientInfo.ip, ex.what());
	}

	INFO("Handler Client{} GetVideoService End...", arg.clientInfo.ip);



}
