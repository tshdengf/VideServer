#include "Epoll.h"
#include "SyncNoBlock.h"
#include "SyncBlock.h"
#include "ThreadPool.h"
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
#include <sys/epoll.h>
using namespace std;

void EpollServer()
{
	cout << "Epoll.." << endl;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	//SetNonBlock(sockfd);

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

	struct epoll_event ev;
	struct epoll_event	all[2000];
	int epfd = epoll_create(2000);

	ev.data.fd = sockfd;
	//ev.events = EPOLLIN | EPOLLET; //边缘触发
	ev.events = EPOLLIN | EPOLLIN;	//水平触发

	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

	while (true)
	{
		int count = epoll_wait(epfd, all, sizeof(all) / sizeof(all[0]), -1);
		if (count <= 0)
			continue;

		//遍历事件
		for (int i = 0; i < count; i++)
		{
			if (all[i].data.fd == sockfd)
			{
				//新连接
				int conn = accept(sockfd, (struct sockaddr*)&client_addr, &length);
				if (conn < 0)
				{
					if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
					{
						continue;
					}

					if (conn == -1)
					{
						perror("accpet");
						exit(1);
					}
				}
				//SetNonBlock(conn);
				string ip = inet_ntoa(client_addr.sin_addr);
				int port = ntohs(client_addr.sin_port);
				cout << "connection:" << ip << "port:" << port << endl;
				//加入监听
				ev.data.fd = conn;
				//ev.events = EPOLLIN | EPOLLET;
				ev.events = EPOLLIN | EPOLLIN;	//水平触发
				epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &ev);
			}
			else
			{
				//客户端有信息交互
				if (!all[i].events & EPOLLIN)
					continue;
				int conn = all[i].data.fd;
				Handler(conn);
				//ThreadPool::Instance()->Run(HandlerNoBlck, conn);
				
				//HandlerNoBlck(conn);

				ev.data.fd = conn;
				//ev.events = EPOLLIN | EPOLLET;
				ev.events = EPOLLIN | EPOLLIN;	//水平触发
				epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &ev);
			}
		}
	}

}