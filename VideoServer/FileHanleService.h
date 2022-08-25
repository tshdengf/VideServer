#pragma once
#include "IRequestHandle.h"
#include "Logger.h"
#include "LibeventServer.h"
#include "VideoServerDao.h"
#include "MsgHeader.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include "CommonUtils.h"

//将变化的封装出来
//策略模式中针对这一级会员的处理
class FileHanleService :
	public IRequestHandle
{
	// 通过 IRequestHandle 继承
	virtual void Process(ProcessArg arg) override;
};

