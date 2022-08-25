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

//���¼�ѭ��
void LibeventServer::DoLoop()
{
	//��ʼ���½����¼��ṹ��
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

	//����Listen ����ص�
	evconnlistener_set_error_cb(listener, VideoServerController::listener_error_cb);

	//�����̳߳ز������������
	ThreadPool::Instance()->Run([this]() {	HeartCheck(); });

	INFO("start event loop!.......");
	//���������¼�ѭ��
	event_base_dispatch(base);
}

LibeventServer::~LibeventServer()
{
	if (listener)
		evconnlistener_free(listener);
	if (base)
		event_base_free(base);
}

//�������� ����ʵ�ʷ��ʹ�С
int LibeventServer::Recv(void * recvHandler, void * recvBuffer, int recvLen)
{
	bufferevent *bev = (bufferevent*)recvHandler;
	return bufferevent_read(bev, recvBuffer, recvLen);
}

//�������� ����ʵ�ʽ��մ�С
int LibeventServer::Send(void * sendHandler, const void * sendBuffer, int sendLen)
{
	bufferevent *bev = (bufferevent*)sendHandler;

	return bufferevent_write(bev, sendBuffer, sendLen);

}
