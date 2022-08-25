#pragma once
#include <unistd.h>
#include <string.h>
#include <event2/listener.h>
#include <iostream>
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include<sys/time.h>
#include <event2/event.h>
void LibEventServer();

void listen_cb(struct evconnlistener *e, evutil_socket_t sock, struct sockaddr *addr, int socklen, void *arg);

void write_cb(struct bufferevent *bev, void *arg);


void event_cb(struct bufferevent *bev, short events, void *arg);


void read_cb(struct bufferevent *bev, void *arg);

