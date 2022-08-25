#pragma once
#include <string>
#include <map>
#include <mutex>
#include <unistd.h>
#include <iostream>
#include "Logger.h"
#include "ClientInfo.h"

//�������ܹ�������
class ServerFrame
{
public:
	//��������������
	virtual bool StartListen(std::string ip, short port, int listenSize = 36) = 0;
	virtual void DoLoop() = 0;

	virtual void  HeartCheck();
	//���µ����Ӳ���map������ͨ�����������ཨ��map�������ļ�����������ϵ
	virtual void  AddNewClient(ClientInfo &client);
	//ͨ�������ļ���ʶ�������������
	virtual void  HandlerHart(int sockfd);
	//ͨ�������ļ���ʶ���Ͽ�����
	virtual void ClientDown(int sockfd);
	//���س������ӵ�������
	virtual ClientInfo GetClient(int sockfd);

	//�� д
	virtual int Recv(void * recvHandler, void * recvBuffer, int recvLen) = 0;
	virtual int Send(void * sendHandler, const void *SendBuffer, int sendLen) = 0;

protected:
	std::string ip;
	short port;
	int listenSize;
	//���пͻ��˼���
	std::map<int, std::pair<ClientInfo, int> > allClient;
	std::mutex headtMutex;
};