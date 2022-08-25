#include "ThreadPool.h"
#include "Logger.h"
#include "ReadConfigFile.h"
#include "LibeventServer.h"
#include "DBConnectionPool.h"
#include "VideoServerDao.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
using namespace std;

void handle_pipe(int sig)
{
	//do nothing
}

int main(int argc, char ** argv)
{
	//处理客户端Pipe
	struct sigaction sa;//sigaction函数的功能是检查或修改与指定信号相关联的处理动作
	sa.sa_handler = handle_pipe;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGPIPE, &sa, NULL);

	try
	{
		//先得到ip和地址
		string ip = ReadConfigFile::ReadValue("server.addr");
		int port = std::stoi(ReadConfigFile::ReadValue("server.port"));

		INFO("server ip:{} port:{}", ip, port);

		//创建线程池
		ThreadPool::Ptr threadPool = ThreadPool::Instance();
		threadPool->Init(ThreadPoolConfig{ 5,  1024, 1024, chrono::seconds(1) });
		threadPool->Start();
		INFO("start threadpool success，size:{}", threadPool->GetTotalThreadSize());

		//初始化数据库
		DBConnectionPool::Instacnce()->Init(
			ReadConfigFile::ReadValue("database.addr"), ReadConfigFile::ReadValue("database.user"), ReadConfigFile::ReadValue("database.pass")
		);

		INFO("start dbpool success size{}", DBConnectionPool::Instacnce()->GetCurSize());
		//实例化基类指针指向派生类
		shared_ptr<ServerFrame> server = std::shared_ptr<ServerFrame>(new LibeventServer());
		//监听得到的本机配置的ip和端口号
		server->StartListen(ip, port);
		//启动服务事件循环
		cout << "start server!" << endl;
		server->DoLoop();

		
	}
	catch (const std::exception& e)
	{
		ERROR("Start Main error{}", e.what());
	}

	return 1;
}