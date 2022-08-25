#pragma once
#include <iostream>


class ReadConfigFile
{
public:
	static std::string ReadValue(std::string key, std::string filepath = "");
};

