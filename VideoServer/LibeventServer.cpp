#include "LibeventServer.h"
#include <unistd.h>
#include <string.h>
#include <event2/listener.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <event2/listener.h>
#include <iostream>
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include<sys/time.h>
#include "Logger.h"
#include "VideoServerController.h"
#include "ThreadPool.h"

bool LibeventServer::StartListen(std::string ip, short port, int listenSize)
{
	this->ip = ip;
	this->port = port;
	this->listenSize = listenSize;
	INFO("StartListen ip {} port {}", ip, port);
	this->base == nullptr;
	this->listener = nullptr;
	return true;
}

//主事件循环
void LibeventServer::DoLoop()
{
	//初始化新建主事件结构体
	this->base = event_base_new();
	if (base == nullptr)
	{
		ERROR("event_base_new error {}", errno);
		return;
	}

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(ip.c_str());

	listener = evconnlistener_new_bind(base, VideoServerController::listen_cb, this, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		listenSize,
		(sockaddr*)&sin,
		sizeof(sin));

	if (listen == NULL)
	{
		ERROR("evconnlistener_new_bind error {}", errno);
		return;
	}

	//设置Listen 错误回调
	evconnlistener_set_error_cb(listener, VideoServerController::listener_error_cb);

	//调用线程池并开启心跳检测
	ThreadPool::Instance()->Run([this]() {	HeartCheck(); });

	INFO("start event loop!.......");
	//阻塞开启事件循环
	event_base_dispatch(base);
}

LibeventServer::~LibeventServer()
{
	if (listener)
		evconnlistener_free(listener);
	if (base)
		event_base_free(base);
}

//发送数据 返回实际发送大小
int LibeventServer::Recv(void * recvHandler, void * recvBuffer, int recvLen)
{
	bufferevent *bev = (bufferevent*)recvHandler;
	return bufferevent_read(bev, recvBuffer, recvLen);
}

//接收数据 返回实际接收大小
int LibeventServer::Send(void * sendHandler, const void * sendBuffer, int sendLen)
{
	bufferevent *bev = (bufferevent*)sendHandler;

	return bufferevent_write(bev, sendBuffer, sendLen);

}
