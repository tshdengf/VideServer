#pragma once

#include "ClientInfo.h"

struct bufferevent;


//ר�ű��洫�����ݵĹ�����
struct ProcessArg
{
	int sockfd;
	//�ͻ��˷��͹���������
	const char * content;	

	//�ͻ�����Ϣ
	ClientInfo clientInfo;

	//��չ�ֶ�
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
