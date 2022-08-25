#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <string>

#include "Logger.h"
#include "ClientInfo.h"
#include "VideoServerController.h"
#include "ServerFrame.h"
#include "LibeventServer.h"
#include "HeartBeatService.h"
#include "ThreadPool.h"
#include "ProcessArg.h"
#include "MsgHeader.h"
#include "LoginService.h"
#include "UpLoadFileService.h"
#include "FileHanleService.h"
#include "GetVideoService.h"

using namespace std;

//被libevent当作回调函数
void VideoServerController::listen_cb(evconnlistener * e, evutil_socket_t sock, sockaddr * addr, int socklen, void * arg)
{
	try
	{
		//调用serverFram类处理
		ServerFrame * service = (LibeventServer*)arg;
		sockaddr_in *client = (sockaddr_in *)addr;
		string ip = inet_ntoa(client->sin_addr);
		unsigned short int port = ntohs(client->sin_port);
		cout << "connection:" << ip << "port:" << port << "sock: " << sock << endl;
		INFO("client connect ip:{} port:{} sock:{}", ip, port, sock);

		//调用函数获取base
		struct event_base *base = evconnlistener_get_base(e);//一键搞定只需监听
		struct bufferevent *buffer_event;
		//为套接字绑定了一块缓冲区，一起发送
		buffer_event = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);

		bufferevent_setcb(buffer_event, read_cb, write_cb, event_cb, service);
		//启动 bufferevent的 读缓冲区。默认是disable 的
		bufferevent_enable(buffer_event, EV_READ);

		//加入客户端集合
		ClientInfo clientInfo(ip, port, (int)sock, buffer_event);
		service->AddNewClient(clientInfo);
	}
	catch (const std::exception& e)
	{
		ERROR("listen cb err{}", e.what());

		cout << e.what() << endl;
	}
	
}

void  VideoServerController::write_cb(struct bufferevent *bev, void *arg)
{
}

void  VideoServerController::event_cb(struct bufferevent *bev, short events, void *arg)
{
	//得到触发回调的文件描述符（句柄）
	int sockfd = bufferevent_getfd(bev);
	//创建一个指针指向服务器框架类（基类指针指向派生类）
	ServerFrame * service = (LibeventServer*)arg;
	if (events & BEV_EVENT_EOF)
	{
		INFO("client{} close", sockfd);
	}
	else if (events & BEV_EVENT_ERROR)
	{
		cout << "some other error !" << endl;
	}
	//断开服务器与这个句柄的连接
	service->ClientDown(sockfd);
	//释放掉bev
	bufferevent_free(bev);
}

//有客户端发消息是触发
void  VideoServerController::read_cb(struct bufferevent *bev, void *arg)
{
	INFO("receive client msg start analysis");
	//用一个基类指针指向派生类
	ServerFrame * service = (LibeventServer*)arg;
	//实例化消息头部
	MsgHeader msgHeader;
	memset(&msgHeader, 0, MSG_HEAD_LEN);
	//接收数据头
	int size = service->Recv((void*)bev, &msgHeader, MSG_HEAD_LEN);

	//接收数据正文
	int sockfd = bufferevent_getfd(bev);//先得到bev文件描述符
	//通信类有文件缓冲区
	auto client = service->GetClient(sockfd);//通过文件描述符调用服务类中得到通信类

	//创建策略模式中抽象会员类指针
	shared_ptr<IRequestHandle> ptr = nullptr;
	try
	{
		//比较消息头前4个字节，即确认标志位
		if (strncmp((const char *)msgHeader.msgHeader, MSG_HEADER, 4) == 0)
		{
			char *buffer = NULL;
			//将bev中的数据悉数读到buffer中
			if (msgHeader.contentLength >= 0)
			{
				buffer = new char[msgHeader.contentLength];
				service->Recv((void *)bev, buffer, msgHeader.contentLength);
			}
			//保存客户端的所有传入数据
			ProcessArg process_arg(sockfd, buffer, client, bev, arg);
			//处理协议
			switch (msgHeader.controlMask)
			{
			case HEART_CHECK:
				//基类指针指向派生类，策略模式，多态的应用
				ptr = shared_ptr<HeartBeatService>(new HeartBeatService());
				ptr->Process(process_arg);
				break;
			case USER_LOGIN:
				ptr = shared_ptr<LoginService>(new LoginService());
				ptr->Process(process_arg);
				break;
			case UPLOAD_FILE:
				//上传文件
				ptr = shared_ptr<UpLoadFileService>(new UpLoadFileService());
				ptr->Process(process_arg);
				break;
			case DELETE_CACHE_FILE:
			case UPLOAD_FILE_OVER:
				ptr = shared_ptr<FileHanleService>(new FileHanleService());
				ptr->Process(process_arg);
				break;
			case GET_VIDEO:
				ptr = shared_ptr<GetVideoService>(new GetVideoService());
				ptr->Process(process_arg);
			default:
				break;
			}
		}
	}
	catch (const std::exception& ex)
	{
		INFO("处理客户端消息异常{}", ex.what());
	}

}

//监听错误回调
void VideoServerController::listener_error_cb(evconnlistener *listener, void * arg)
{
	event_base *base = evconnlistener_get_base(listener);

	int err = EVUTIL_SOCKET_ERROR();

	ERROR("listener error code {} msg{}", err, evutil_socket_error_to_string(err));

	event_base_loopexit(base, NULL);
}
