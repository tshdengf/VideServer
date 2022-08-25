#pragma once
#include "IRequestHandle.h"
struct  ProcessArg;


//策略模式中针对这一级会员的处理
class HeartBeatService :
	public IRequestHandle
{
	// 通过 IRequestHandle 继承
	virtual void Process(ProcessArg arg) override;
};

