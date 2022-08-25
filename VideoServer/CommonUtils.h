#pragma once

#include <iostream>
#include <memory>
using namespace std;

//用于json和普通数据之间的转换
class CommonUtils
{
public:
	template<typename ... Args>
	static string string_format(const string& format, Args ... args);


	static unsigned long get_file_size(const char *path);
};

template<typename ...Args>
inline string CommonUtils::string_format(const string & format, Args ...args)
{
	size_t size = 1 + snprintf(nullptr, 0, format.c_str(), args ...);
	char bytes[size];
	snprintf(bytes, size, format.c_str(), args ...);
	return string(bytes);
}
