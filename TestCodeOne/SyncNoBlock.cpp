#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <thread>
#include <string>
#include <iostream>
using namespace std;

#include "SyncNoBlock.h"
#include "ThreadPool.h"


//·Ç×èÈû



int SetNonBlock(int iSock)
{
	int iFlags;

	iFlags = fcntl(iSock, F_GETFL, 0);
	iFlags |= O_NONBLOCK;
	iFlags |= O_NDELAY;
	int ret = fcntl(iSock, F_SETFL, iFlags);
	return ret;
}

void SyncNoBlockServer()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	//ÉèÖÃ·Ç×èÈû
	SetNonBlock(sockfd);

	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(8090);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	struct sockaddr_in client_addr;


	socklen_t length = sizeof(client_addr);

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}

	listen(sockfd, 36);

	while (true)
	{
		int conn = accept(sockfd, (struct sockaddr*)&client_addr, &length);
		if (conn < 0)
		{
			if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
			{
				continue;
			}

			if (conn == 1)
			{
				perror("accept");
				exit(1);
			}
		}
		string ip = inet_ntoa(client_addr.sin_addr);
		int port = ntohs(client_addr.sin_port);
		cout << "connection:" << ip << "port:" << port << endl;

		SetNonBlock(conn);

		/*thread t1(HandlerNoBlck, conn);
		t1.detach();*/
		HandlerNoBlck(conn);
		//ThreadPool::Instance()->Run(HandlerNoBlck, conn);

	}
}

void HandlerNoBlck(int fd)
{
	char buffer[1024] = "\0";

	//while (true)
	//{
		int len = Recv(fd, buffer, 7);
		if (len == 0)
		{
			close(fd);
			return;
		}
		else if (len != -1)
		{
			int ret = send(fd, "\r\n", 2, 0);
			if (ret == -1)
			{
				cout << "send error" << endl;
			}
			else if (ret == 0)
			{
				close(fd);
				return;
			}
		}
	//}
}

int Send(int sockfd, const char *buf, int sendSize)
{
	int bytes = -1;

	int length = sendSize;
	do
	{
	ReSend:
		bytes = send(sockfd, buf, length, 0);
		if (bytes > 0)
		{
			length -= bytes;
			buf += bytes;
		}
		else if (bytes < 0)
		{
			if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
				goto ReSend;

			if (bytes == -1)
			{
				return -1;
			}
		}
		else if (bytes == 0)
		{
			return 0;
		}
	} while (length > 0 && bytes > 0);

	if (length == 0)
		bytes = sendSize;
	return bytes;
}

int Recv(int sockfd, char *buf, int recvSize)
{
	int recvLen = 0;
	int nRet = 0;

	while (recvLen < recvSize)
	{
		nRet = recv(sockfd, buf, recvSize - recvLen, 0);
		if (nRet > 0)
		{
			recvLen += nRet;
			buf += nRet;
		}
		else if (nRet < 0)
		{
			if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) {
				continue;
			}
		}
		else
		{
			return 0;
		}

	}
	return nRet;
}