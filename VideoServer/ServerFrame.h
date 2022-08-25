#pragma once
#include <string>
#include <map>
#include <mutex>
#include <unistd.h>
#include <iostream>
#include "Logger.h"
#include "ClientInfo.h"

//服务器架构抽象类
class ServerFrame
{
public:
	//服务器启动监听
	virtual bool StartListen(std::string ip, short port, int listenSize = 36) = 0;
	virtual void DoLoop() = 0;

	virtual void  HeartCheck();
	//将新的连接插入map容器，通过传入连接类建立map容器与文件描述符的联系
	virtual void  AddNewClient(ClientInfo &client);
	//通过传入文件标识符重置心跳检测
	virtual void  HandlerHart(int sockfd);
	//通过传入文件标识符断开连接
	virtual void ClientDown(int sockfd);
	//返回出这连接的连接类
	virtual ClientInfo GetClient(int sockfd);

	//读 写
	virtual int Recv(void * recvHandler, void * recvBuffer, int recvLen) = 0;
	virtual int Send(void * sendHandler, const void *SendBuffer, int sendLen) = 0;

protected:
	std::string ip;
	short port;
	int listenSize;
	//所有客户端集合
	std::map<int, std::pair<ClientInfo, int> > allClient;
	std::mutex headtMutex;
};