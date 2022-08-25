#pragma once

#include<string>
#include <event2/bufferevent.h>
using namespace std;

//���������ӵ���
struct ClientInfo
{
	//�ͻ���IP
	std::string ip;
	//�˿ں�
	unsigned short int port;
	//�׽���
	int sockfd;
	//��д������
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
