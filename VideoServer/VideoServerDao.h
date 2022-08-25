#pragma once


#include <list>
#include <unordered_map>
#include <string>
#include "DBConnectionPool.h"
#include<cppconn/prepared_statement.h>
#include "Logger.h"


using namespace std;
using namespace sql;


//一个递归的展开函数
//对不同传参的函数重载
inline void ExpanArgs(int index, PreparedStatement * preStatement)
{

}
//一个递归的展开函数
template <class T>
inline void ExpanArgs(int index, PreparedStatement * preStatement, T arg)
{
	preStatement->setString(index, string((const char *)arg));
}
//一个递归的展开数据的函数
template <class T, class ...Args>
inline void ExpanArgs(unsigned int index, PreparedStatement * preStatement, T head, Args... args)
{
	//向PreparedStatement类中添加数据
	preStatement->setString(index, string((const char *)head));
	ExpanArgs(++index, preStatement, args...);
}

//返回值是一个列表（保存无符号的map），传入一个查询语句
//没有参数化查询
inline list<unordered_map<string, string>> ExecuteQueryNoParam(string sql)
{
	list<unordered_map<string, string>> result;
	try
	{
		//创建一个对象后获取对象
		auto conn = DBConnectionPool::Instacnce()->GetConnection();
		//Statement对象，用于执行不带参数的简单SQL语句。
		Statement *stmt = conn->createStatement();
		//执行查询语句sql
		ResultSet* res = stmt->executeQuery(sql);
		while (res->next())
		{
			//实例化一个容器
			unordered_map<string, string> mapRes;
			//得到数据库查询数据
			auto rsmd = res->getMetaData();
			//得到列数
			for (size_t i = 1; i < rsmd->getColumnCount(); i++)
			{
				//将每一列插入容器
				mapRes.insert({ rsmd->getColumnLabel(i), res->getString(i) });
			}
			result.push_back(mapRes);
		}
		//查询完放回连接
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
		//展开参数
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

		//查询完放回连接
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
		//展开参数
		ExpanArgs(1, preStatement, head, args...);
		int count = preStatement->executeUpdate();
		INFO("excute sql cure {} data", count);
	}
	catch (const std::exception& e)
	{
		INFO("SQL:{} error{}", sql, e.what());
	}

}


