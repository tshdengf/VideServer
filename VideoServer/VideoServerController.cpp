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

//��libevent�����ص�����
void VideoServerController::listen_cb(evconnlistener * e, evutil_socket_t sock, sockaddr * addr, int socklen, void * arg)
{
	try
	{
		//����serverFram�ദ��
		ServerFrame * service = (LibeventServer*)arg;
		sockaddr_in *client = (sockaddr_in *)addr;
		string ip = inet_ntoa(client->sin_addr);
		unsigned short int port = ntohs(client->sin_port);
		cout << "connection:" << ip << "port:" << port << "sock: " << sock << endl;
		INFO("client connect ip:{} port:{} sock:{}", ip, port, sock);

		//���ú�����ȡbase
		struct event_base *base = evconnlistener_get_base(e);//һ���㶨ֻ�����
		struct bufferevent *buffer_event;
		//Ϊ�׽��ְ���һ�黺������һ����
		buffer_event = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);

		bufferevent_setcb(buffer_event, read_cb, write_cb, event_cb, service);
		//���� bufferevent�� ����������Ĭ����disable ��
		bufferevent_enable(buffer_event, EV_READ);

		//����ͻ��˼���
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
	//�õ������ص����ļ��������������
	int sockfd = bufferevent_getfd(bev);
	//����һ��ָ��ָ�����������ࣨ����ָ��ָ�������ࣩ
	ServerFrame * service = (LibeventServer*)arg;
	if (events & BEV_EVENT_EOF)
	{
		INFO("client{} close", sockfd);
	}
	else if (events & BEV_EVENT_ERROR)
	{
		cout << "some other error !" << endl;
	}
	//�Ͽ���������������������
	service->ClientDown(sockfd);
	//�ͷŵ�bev
	bufferevent_free(bev);
}

//�пͻ��˷���Ϣ�Ǵ���
void  VideoServerController::read_cb(struct bufferevent *bev, void *arg)
{
	INFO("receive client msg start analysis");
	//��һ������ָ��ָ��������
	ServerFrame * service = (LibeventServer*)arg;
	//ʵ������Ϣͷ��
	MsgHeader msgHeader;
	memset(&msgHeader, 0, MSG_HEAD_LEN);
	//��������ͷ
	int size = service->Recv((void*)bev, &msgHeader, MSG_HEAD_LEN);

	//������������
	int sockfd = bufferevent_getfd(bev);//�ȵõ�bev�ļ�������
	//ͨ�������ļ�������
	auto client = service->GetClient(sockfd);//ͨ���ļ����������÷������еõ�ͨ����

	//��������ģʽ�г����Ա��ָ��
	shared_ptr<IRequestHandle> ptr = nullptr;
	try
	{
		//�Ƚ���Ϣͷǰ4���ֽڣ���ȷ�ϱ�־λ
		if (strncmp((const char *)msgHeader.msgHeader, MSG_HEADER, 4) == 0)
		{
			char *buffer = NULL;
			//��bev�е�����Ϥ������buffer��
			if (msgHeader.contentLength >= 0)
			{
				buffer = new char[msgHeader.contentLength];
				service->Recv((void *)bev, buffer, msgHeader.contentLength);
			}
			//����ͻ��˵����д�������
			ProcessArg process_arg(sockfd, buffer, client, bev, arg);
			//����Э��
			switch (msgHeader.controlMask)
			{
			case HEART_CHECK:
				//����ָ��ָ�������࣬����ģʽ����̬��Ӧ��
				ptr = shared_ptr<HeartBeatService>(new HeartBeatService());
				ptr->Process(process_arg);
				break;
			case USER_LOGIN:
				ptr = shared_ptr<LoginService>(new LoginService());
				ptr->Process(process_arg);
				break;
			case UPLOAD_FILE:
				//�ϴ��ļ�
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
		INFO("����ͻ�����Ϣ�쳣{}", ex.what());
	}

}

//��������ص�
void VideoServerController::listener_error_cb(evconnlistener *listener, void * arg)
{
	event_base *base = evconnlistener_get_base(listener);

	int err = EVUTIL_SOCKET_ERROR();

	ERROR("listener error code {} msg{}", err, evutil_socket_error_to_string(err));

	event_base_loopexit(base, NULL);
}
