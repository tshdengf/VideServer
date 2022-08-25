#pragma once


#include <list>
#include <unordered_map>
#include <string>
#include "DBConnectionPool.h"
#include<cppconn/prepared_statement.h>
#include "Logger.h"


using namespace std;
using namespace sql;


//һ���ݹ��չ������
//�Բ�ͬ���εĺ�������
inline void ExpanArgs(int index, PreparedStatement * preStatement)
{

}
//һ���ݹ��չ������
template <class T>
inline void ExpanArgs(int index, PreparedStatement * preStatement, T arg)
{
	preStatement->setString(index, string((const char *)arg));
}
//һ���ݹ��չ�����ݵĺ���
template <class T, class ...Args>
inline void ExpanArgs(unsigned int index, PreparedStatement * preStatement, T head, Args... args)
{
	//��PreparedStatement�����������
	preStatement->setString(index, string((const char *)head));
	ExpanArgs(++index, preStatement, args...);
}

//����ֵ��һ���б������޷��ŵ�map��������һ����ѯ���
//û�в�������ѯ
inline list<unordered_map<string, string>> ExecuteQueryNoParam(string sql)
{
	list<unordered_map<string, string>> result;
	try
	{
		//����һ��������ȡ����
		auto conn = DBConnectionPool::Instacnce()->GetConnection();
		//Statement��������ִ�в��������ļ�SQL��䡣
		Statement *stmt = conn->createStatement();
		//ִ�в�ѯ���sql
		ResultSet* res = stmt->executeQuery(sql);
		while (res->next())
		{
			//ʵ����һ������
			unordered_map<string, string> mapRes;
			//�õ����ݿ��ѯ����
			auto rsmd = res->getMetaData();
			//�õ�����
			for (size_t i = 1; i < rsmd->getColumnCount(); i++)
			{
				//��ÿһ�в�������
				mapRes.insert({ rsmd->getColumnLabel(i), res->getString(i) });
			}
			result.push_back(mapRes);
		}
		//��ѯ��Ż�����
		DBConnectionPool::Instacnce()->ReleaseConnection(conn);
	}
	catch (const std::exception& e)
	{
		INFO("SQL:{} error{}", sql, e.what());
		throw e;
	}


	return result;
}

template <class T, class ...Args>
inline list<unordered_map<string, string>> ExecuteQuery(string sql, T head, Args...args)
{
	list<unordered_map<string, string>> result;
	try
	{
		auto conn = DBConnectionPool::Instacnce()->GetConnection();
		PreparedStatement *preStatement = conn->prepareStatement(sql);
		//չ������
		ExpanArgs(1, preStatement, head, args...);

		ResultSet* res = preStatement->executeQuery();
		while (res->next())
		{
			unordered_map<string, string> mapRes;
			auto rsmd = res->getMetaData();
			for (size_t i = 1; i < rsmd->getColumnCount(); i++)
			{
				mapRes.insert({ rsmd->getColumnLabel(i), res->getString(i) });
			}
			result.push_back(mapRes);
		}

		//��ѯ��Ż�����
		DBConnectionPool::Instacnce()->ReleaseConnection(conn);
	}
	catch (const std::exception&e)
	{
		INFO("SQL:{} error{}", sql, e.what());
		throw e;
	}
	return result;
}

template <class T, class ...Args>
inline void ExecuteSQL(string sql, T head, Args...args)
{
	try
	{
		auto conn = DBConnectionPool::Instacnce()->GetConnection();
		PreparedStatement *preStatement = conn->prepareStatement(sql);
		//չ������
		ExpanArgs(1, preStatement, head, args...);
		int count = preStatement->executeUpdate();
		INFO("excute sql cure {} data", count);
	}
	catch (const std::exception& e)
	{
		INFO("SQL:{} error{}", sql, e.what());
	}

}


