#pragma once

#include "ServerFrame.h"
#include <map>
#include <mutex>
#include <event2/event.h>
#include "ClientInfo.h"


//ר�Ÿ���libevent�����Ǽ�����ѭ��������д��
struct event_base;
struct evconnlistener;
class LibeventServer :
	public ServerFrame
{
public:
	// ͨ�� ServerFrame �̳�
	virtual bool StartListen(std::string ip, short port, int listenSize = 36) override;
	virtual void DoLoop() override;

	virtual int Recv(void * recvHandler, void * recvBuffer, int recvLen) override;
	//������ͻ���д����
	virtual int Send(void * sendHandler, const void * SendBuffer, int sendLen) override;

	~LibeventServer();

private:
	event_base *base;
	evconnlistener * listener;

	// ͨ�� ServerFrame �̳�

};

