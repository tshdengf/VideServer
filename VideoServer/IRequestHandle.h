#pragma once

struct  ProcessArg;

//策略模式中被抽象的会员
//处理客户端请求 抽象类
class IRequestHandle
{
public:
	virtual void Process(ProcessArg arg) = 0;
};

