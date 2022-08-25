#include "LoginService.h"
#include "Logger.h"
#include "CommonUtils.h"
#include "MsgHeader.h"
#include "LibeventServer.h"
#include "VideoServerDao.h"
#include <iostream>
#include <string>

using namespace std;

void LoginService::Process(ProcessArg arg)
{
	INFO("处理客户端{} 登录", arg.clientInfo.ip);
	ServerFrame * service = (LibeventServer*)arg.argExtend;
	VerifyLoginInfo verifyInfo;
	MsgHeader head(sizeof(verifyInfo), 0);
	char sendBuffer[MSG_HEAD_LEN + sizeof(verifyInfo)] = "\0";
	memcpy(sendBuffer, &head, MSG_HEAD_LEN);
	try
	{
		string sql = "select ob_object_id from user where user_phone_num = ? and user_password = ?";

		LoginInfo loginInfo;

		memcpy(&loginInfo, arg.content, sizeof(loginInfo));

		auto queryRes = ExecuteQuery(sql, loginInfo.username, loginInfo.password);
		if (queryRes.size() == 0)
		{
			verifyInfo.success = false;
			strcpy((char *)verifyInfo.errorMsg, "User Name Or Password Error");
			memcpy(sendBuffer + MSG_HEAD_LEN, &verifyInfo, sizeof(verifyInfo));
			service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN + sizeof(verifyInfo));

			sql = "insert into login_record (USER_PHTON_NUM, LOGIN_IP, STATUS) values (?,?,?)";
			ExecuteSQL(sql, loginInfo.username, loginInfo.ip, "0");

			return;
		}
		verifyInfo.success = true;
		memcpy(sendBuffer + MSG_HEAD_LEN, &verifyInfo, sizeof(verifyInfo));
		service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN + sizeof(verifyInfo));

		//插入登录记录表
		sql = "insert into login_record (USER_PHTON_NUM, LOGIN_IP, STATUS) values (?,?,?)";
		const char * value = (verifyInfo.success) ? "1" : "0";
		ExecuteSQL(sql, loginInfo.username, loginInfo.ip, value);
	}
	catch (const std::exception& ex)
	{
		verifyInfo.success = false;
		strcpy((char *)verifyInfo.errorMsg, "Server Exception");
		memcpy(sendBuffer + MSG_HEAD_LEN, &verifyInfo, sizeof(verifyInfo));
		service->Send(arg.bev, sendBuffer, MSG_HEAD_LEN + sizeof(verifyInfo));
	}
}
