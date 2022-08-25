#pragma once
#include<mysql_connection.h>
#include<mysql_driver.h>
#include<cppconn/exception.h>
#include<cppconn/driver.h>
#include<cppconn/connection.h>
#include<cppconn/resultset.h>
#include<cppconn/prepared_statement.h>
#include<cppconn/statement.h>
#include <mutex>
#include<list>

//数据库连接池
class DBConnectionPool
{
public:
	using Ptr = std::shared_ptr<DBConnectionPool>;

	~DBConnectionPool();

	//获取数据库连接对象
	sql::Connection *GetConnection();
	//插入一个连接
	void ReleaseConnection(sql::Connection *conn);

	static Ptr Instacnce();

	void Init(std::string url, std::string userName, std::string password, int maxSize = 10);

	inline int GetCurSize() { return this->curSize; }
	inline int GetMaxSize() { return this->maxSize; }

private:
	DBConnectionPool() {};
	DBConnectionPool(const DBConnectionPool&) = delete;
	DBConnectionPool& operator=(const DBConnectionPool&) = delete;

	void DestoryConnection(sql::Connection * conn);

	void DestoryConnectionPool();

	//创建一个连接用于给外部使用
	sql::Connection* CreateConnection();

	//初始化数据库连接池
	void InitConnectionPool(int iInitSize);

private:
	static std::mutex m_mutex;

	int curSize;	//当前已经建立连接的数量
	int maxSize; //连接池中定义的最大数据库连接数
	std::string user;
	std::string password;
	std::string url;
	std::list<sql::Connection*> connList; //连接池的容器队列  STL list 双向链表
	std::mutex mutex; //线程锁
	static Ptr m_instance_ptr;
	sql::Driver*driver;

	std::mutex list_mutex;

};

