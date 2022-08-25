#pragma once
struct evconnlistener;
struct sockaddr;
struct evconnlistener;
#define evutil_socket_t int

//负责用回调函数从客户端读数据
class VideoServerController
{
public:
	void static listen_cb(struct evconnlistener * e, evutil_socket_t sock, struct sockaddr *addr, int socklen, void *arg);

	void static write_cb(struct bufferevent *bev, void *arg);


	void static event_cb(struct bufferevent *bev, short events, void *arg);


	void static read_cb(struct bufferevent *bev, void *arg);

	void static listener_error_cb(struct evconnlistener *, void *);
};




