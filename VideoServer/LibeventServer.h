#pragma once

#include "ServerFrame.h"
#include <map>
#include <mutex>
#include <event2/event.h>
#include "ClientInfo.h"


//专门负责libevent关于是监听，循环，读，写。
struct event_base;
struct evconnlistener;
class LibeventServer :
	public ServerFrame
{
public:
	// 通过 ServerFrame 继承
	virtual bool StartListen(std::string ip, short port, int listenSize = 36) override;
	virtual void DoLoop() override;

	virtual int Recv(void * recvHandler, void * recvBuffer, int recvLen) override;
	//主动向客户端写数据
	virtual int Send(void * sendHandler, const void * SendBuffer, int sendLen) override;

	~LibeventServer();

private:
	event_base *base;
	evconnlistener * listener;

	// 通过 ServerFrame 继承

};

