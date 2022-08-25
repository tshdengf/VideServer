#include "ReadConfigFile.h"
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>
#include "Logger.h"
#include <iostream>

using namespace std;
using namespace libconfig;


//读取配置项
string ReadConfigFile::ReadValue(std::string key, std::string filepath)
{
	if (key.empty()) return "";

	if (filepath.empty()) filepath = "./config/app.conf";

	INFO("读取配置文件文件:{}", filepath);

	Config cfg;

	//解析配置文件
	try
	{
		cfg.readFile(filepath);
		string value = cfg.lookup(key);
		return value;
	}
	catch (const std::exception& e)
	{
		ERROR("读取配置信息异常:{}", e.what());
		throw e;
	}
}



