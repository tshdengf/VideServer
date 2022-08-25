#pragma once

#include<string>
#include <event2/bufferevent.h>
using namespace std;

//负责建立连接的类
struct ClientInfo
{
	//客户端IP
	std::string ip;
	//端口号
	unsigned short int port;
	//套接字
	int sockfd;
	//读写缓冲区
	bufferevent* buffer;

	ClientInfo(std::string ip, unsigned short int port, int sockfd, bufferevent* buf)
	{
		this->ip = ip;
		this->port = port;
		this->sockfd = sockfd;
		this->buffer = buf;
	}

	ClientInfo(const ClientInfo &client)
	{
		this->ip = client.ip;
		this->port = client.port;
		this->sockfd = client.sockfd;
		this->buffer = client.buffer;
	}

	ClientInfo& operator=(const ClientInfo& client)
	{
		this->ip = client.ip;
		this->port = client.port;
		this->sockfd = client.sockfd;
		this->buffer = client.buffer;
	}
	
	ClientInfo()
	{
			
	}
};
