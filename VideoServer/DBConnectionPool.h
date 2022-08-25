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

//���ݿ����ӳ�
class DBConnectionPool
{
public:
	using Ptr = std::shared_ptr<DBConnectionPool>;

	~DBConnectionPool();

	//��ȡ���ݿ����Ӷ���
	sql::Connection *GetConnection();
	//����һ������
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

	//����һ���������ڸ��ⲿʹ��
	sql::Connection* CreateConnection();

	//��ʼ�����ݿ����ӳ�
	void InitConnectionPool(int iInitSize);

private:
	static std::mutex m_mutex;

	int curSize;	//��ǰ�Ѿ��������ӵ�����
	int maxSize; //���ӳ��ж����������ݿ�������
	std::string user;
	std::string password;
	std::string url;
	std::list<sql::Connection*> connList; //���ӳص���������  STL list ˫������
	std::mutex mutex; //�߳���
	static Ptr m_instance_ptr;
	sql::Driver*driver;

	std::mutex list_mutex;

};

