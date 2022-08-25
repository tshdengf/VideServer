#include "ServerFrame.h"

void ServerFrame::HeartCheck()
{
	INFO("start HeartCheck thread");
	while (true)
	{
		//加锁
		{
			std::lock_guard<std::mutex> lock(headtMutex);
			auto it = allClient.begin();
			while (it != allClient.end())
			{
				if (it->second.second == 5)
				{
					INFO("15s had not receive client:{} ip:{} port:{} heart，judge close....", it->second.first.sockfd, it->second.first.ip, it->second.first.port);
					close(it->first);
					allClient.erase(it++);
				}
				else if (it->second.second < 5 && it->second.second >= 0)
				{
					it->second.second += 1;
					it++;
				}
				else
				{
					it++;
				}
			}
		}
		sleep(60);
	}
}

void ServerFrame::AddNewClient(ClientInfo &client)
{
	try
	{
		std::lock_guard<std::mutex> lock(headtMutex);
		allClient.insert(std::make_pair(client.sockfd, std::make_pair(std::move(client), 0)));
	}
	catch (const std::exception& e)
	{
		INFO("HandlerHart error{}", e.what());
	}
	
}

void ServerFrame::HandlerHart(int sockfd)
{
	try
	{
		std::lock_guard<std::mutex> lock(headtMutex);
		allClient[sockfd].second = 0;
	}
	catch (const std::exception& e)
	{
		INFO("HandlerHart error{}", e.what());
	}


}

void ServerFrame::ClientDown(int sockfd)
{
	try
	{
		std::lock_guard<std::mutex> lock(headtMutex);
		allClient.erase(sockfd);
	}
	catch (const std::exception& e)
	{
		INFO("ClientDown error{}", e.what());
	}

}

ClientInfo ServerFrame::GetClient(int sockfd)
{
	try
	{
		std::lock_guard<std::mutex> lock(headtMutex);
		//获得下标的值
		auto value = allClient.at(sockfd);
		//返回下标值的第一个
		return std::move(value.first);
	}
	catch (const std::exception& e)
	{
		INFO("GetClient error{}", e.what());
	}
}
