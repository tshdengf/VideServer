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
#include "SyncBlock.h"
#include "ThreadPool.h"
using namespace std;


//同步阻塞 多线程
void SyncBlockServer()
{
	cout << "SyncBlockServer" << endl;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

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
		if (conn == 1)
		{
			perror("accept");
			exit(1);
		}
		string ip = inet_ntoa(client_addr.sin_addr);
		int port = ntohs(client_addr.sin_port);
		cout << "connection:" << ip << "port:" << port << endl;

		ThreadPool::Instance()->Run(Handler, conn);
	}

}



void Handler(int fd)
{
	char buffer[1024] = "\0";
	int len = recv(fd, buffer, sizeof(buffer), 0);
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
	
}


