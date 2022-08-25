#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

struct ClientInfo
{
	//客户端IP
	std::string ip;
	//端口号
	unsigned short int port;
	//套接字
	int sockfd;
	//读写缓冲区
	void* buffer;

	ClientInfo(std::string ip, unsigned short int port, int sockfd, void* buf)
	{
		cout << "args..." << endl;
		this->ip = ip;
		this->port = port;
		this->sockfd = sockfd;
		this->buffer = buf;
	}

	ClientInfo(const ClientInfo &client)
	{
		cout << "copy.." << endl;
		this->ip = client.ip;
		this->port = client.port;
		this->sockfd = client.sockfd;
		this->buffer = client.buffer;
	}

	ClientInfo& operator=(const ClientInfo& client)
	{
		cout << "operator =" << endl;
		this->ip = client.ip;
		this->port = client.port;
		this->sockfd = client.sockfd;
		this->buffer = client.buffer;
	}

	ClientInfo()
	{
		cout << "now arg.." << endl;
	}

	ClientInfo(ClientInfo&& client)
	{
		cout << "move" << endl;
		this->ip = client.ip;
		this->port = client.port;
		this->sockfd = client.sockfd;
		this->buffer = client.buffer;
	}
};

std::map<int, std::pair<ClientInfo, int> > allClient;

ClientInfo GetClient1(int sockfd)
{
	auto value = allClient.at(sockfd);

	return std::move(value.first);
}

//有问题！！！！ 局部变量不可作为 函数的引用返回值
ClientInfo& GetClient2(int sockfd)
{
	auto value = allClient.at(sockfd);

	return value.first;
}


ClientInfo GetClient5(int sockfd)
{
	auto value = allClient.at(sockfd);
	return value.first;
}


/*
args...
args...
args...
move
move
move
move
move
move
move
move
move
--------------------------
copy..
move
--------------------------
copy..
copy..
--------------------------
*/

int main(int argc, char** argv)
{
	char buffer[1024] = "112233\0";

	char buffer2[1024] = "11223344\0";

	char buffer3[1024] = "1122334455\0";

	ClientInfo c1("10.200.126.196", 5555, 3, &buffer);
	ClientInfo c2("10.200.126.197", 5556, 4, &buffer2);
	ClientInfo c3("10.200.126.198", 5557, 5, &buffer3);

	//在C++11中使用make_pair一定不要显式给出类型参数，不然会报错
	//，C++11中的make_pair使用的是万能引用，使用<int，int>实例化模板会将类型推导为int&&，
	//也就是右值引用，而传入的m和n是左值，所以报错。auto x=make_pair<int,int>(0,0);就是正确的，
	//或者auto x=make_pair<int&,int&>(m,n);T推导为T& &&，折叠为T&，而传入的m和n是左值
	allClient.insert(std::make_pair(3, std::make_pair(c1, 0)));
	allClient.insert(std::make_pair(4, std::make_pair(c2, 0)));
	allClient.insert(std::make_pair(5, std::make_pair(c3, 0)));

	cout << "--------------------------" << endl;
	auto client = GetClient1(3);
	cout << "--------------------------" << endl;
	auto clien3 = GetClient5(3);
	cout << "--------------------------" << endl;
	//auto clien2 = GetClient2(3);

}
