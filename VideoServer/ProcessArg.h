#pragma once

#include "ClientInfo.h"

struct bufferevent;


//专门保存传入数据的工具类
struct ProcessArg
{
	int sockfd;
	//客户端发送过来的数据
	const char * content;	

	//客户端信息
	ClientInfo clientInfo;

	//拓展字段
	void *argExtend;

	struct bufferevent* bev;

	ProcessArg(int sockfd, const char * content, ClientInfo &clientInfo, struct bufferevent * bev, void *argExtend)
	{
		this->sockfd = sockfd;
		this->content = content;
		this->clientInfo = clientInfo;
		this->argExtend = argExtend;
		this->bev = bev;
	}
}; 
