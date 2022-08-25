#include "Test.h"
#include "ThreadPool.h"

using namespace std;

void LibEventServer()
{
	cout << "LibEvent server..." << endl;
	event_base *base = event_base_new();
	if (base == NULL)
	{
		perror("event_base");
		exit(1);
	}


	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(8090);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	evconnlistener * listener = evconnlistener_new_bind(base, listen_cb, base, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		36,
		(sockaddr*)&server_sockaddr,
		sizeof(server_sockaddr));
	if (listener == NULL)
	{
		perror("evconnlistener_new_bind");
		exit(1);
	}

	event_base_dispatch(base);
}

void listen_cb(struct evconnlistener *e, evutil_socket_t sock, struct sockaddr *addr, int socklen, void *arg)
{
	sockaddr_in *client = (sockaddr_in *)addr;
	string ip = inet_ntoa(client->sin_addr);
	int port = ntohs(client->sin_port);
	cout << "connection:" << ip << "port:" << port << endl;

	struct event_base *base = (struct event_base *)arg;
	struct bufferevent *buffer_event;
	//为套接字绑定了一块缓冲区，一起发送
	buffer_event = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(buffer_event, read_cb, write_cb, event_cb, (void *)&sock);
	//启动 bufferevent的 读缓冲区。默认是disable 的
	bufferevent_enable(buffer_event, EV_READ);

}

void write_cb(struct bufferevent *bev, void *arg)
{
}

void event_cb(struct bufferevent *bev, short events, void *arg)
{
	int sockfd = bufferevent_getfd(bev);
	if (events & BEV_EVENT_EOF)
	{
	}
	else if (events & BEV_EVENT_ERROR)
	{
		cout << "some other error !" << endl;
	}

	bufferevent_free(bev);
}

void read_cb(struct bufferevent *bev, void *arg)
{
	char buffer[1024] = "\0";
	bufferevent_read(bev, buffer, 7);

	if (-1 == bufferevent_write(bev, "11\r\n", 4))
	{
		cout << errno << endl;
	}
}