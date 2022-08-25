#include "DBConnectionPool.h"
#include "Logger.h"

using namespace std;
using namespace sql;

using Ptr = std::shared_ptr<DBConnectionPool>;
Ptr DBConnectionPool::m_instance_ptr = nullptr;

std::mutex DBConnectionPool::m_mutex;

DBConnectionPool::~DBConnectionPool()
{
	this->DestoryConnectionPool();
}

//��ȡ����
sql::Connection * DBConnectionPool::GetConnection()
{
	Connection *conn;
	lock_guard<std::mutex> lock(list_mutex);
	INFO("DBConnectionPool now size:", curSize);
	if (connList.size() > 0)
	{
		conn = connList.front();
		connList.pop_front();
		if (conn->isClosed())
		{
			delete conn;
			conn = CreateConnection();
		}
		if (conn == nullptr)
			--curSize;
		return conn;
	}
	else
	{
		if (curSize < maxSize)
		{
			conn = CreateConnection();
			if (conn)
			{
				++curSize;
				return conn;
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}
}

//����һ�����ӵ�������
void DBConnectionPool::ReleaseConnection(sql::Connection * conn)
{
	if (conn)
	{
		lock_guard<std::mutex> lock(list_mutex);
		connList.push_back(conn);
	}
}

//����һ�����ݿ�ʵ��
Ptr DBConnectionPool::Instacnce()
{
	if (m_instance_ptr == nullptr)
	{
		lock_guard<std::mutex> lock(m_mutex);
		if (m_instance_ptr == nullptr)
		{
			m_instance_ptr = std::shared_ptr<DBConnectionPool>(new DBConnectionPool);
		}
	}
	return m_instance_ptr;
}

//��ʼ���̳߳�
void DBConnectionPool::Init(std::string url, std::string userName, std::string password, int maxSize)
{
	this->maxSize = maxSize;
	this->curSize = 0;
	this->user = user;
	this->url = url;
	this->password = password;
	try
	{
		this->driver = sql::mysql::get_driver_instance();
	}
	catch (const std::exception& e)
	{
		ERROR("Init DBConnectionPool:{}", e.what());
		throw e;
	}
	this->InitConnectionPool(maxSize / 2);
}

//��������
void DBConnectionPool::DestoryConnection(sql::Connection * conn)
{
	if (conn)
	{
		try
		{
			conn->close();
		}
		catch (sql::SQLException &e)
		{
			ERROR("DestoryConnection SQLEX:{}",e.what());
		}
		catch (std::exception &e)
		{
			ERROR("DestoryConnection exception:{}", e.what());
		}
		delete conn;
	}
}

//�������ӳ�
void DBConnectionPool::DestoryConnectionPool()
{
	lock_guard<std::mutex> lock(list_mutex);
	list<Connection *>::iterator iter;
	for (iter = connList.begin(); iter != connList.end(); ++iter)
		this->DestoryConnection(*iter);
	curSize = 0;
	connList.clear();
}

//�������ݿ����� ��λ
sql::Connection * DBConnectionPool::CreateConnection()
{
	Connection *conn;
	try
	{
		conn = driver->connect(url, user, password); //��������
		conn->setSchema("eve_db");
		INFO("connection db{} {} success", url, user);
		return conn;
	}
	catch (sql::SQLException &e)
	{
		ERROR(e.what());
		return nullptr;
	}
	catch (std::runtime_error &e)
	{
		ERROR(e.what());
		return nullptr;
	}
}

void DBConnectionPool::InitConnectionPool(int iInitSize)
{
	Connection *conn;
	lock_guard<std::mutex> lock(list_mutex);
	for (int i = 0; i < iInitSize; ++i)
	{
		conn = CreateConnection();
		if (conn)
		{
			connList.push_back(conn);
			++curSize;
		}
		else
		{
			ERROR("InitConnectionPool create conn error");
		}
	}
}
