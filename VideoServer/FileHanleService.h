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

//���仯�ķ�װ����
//����ģʽ�������һ����Ա�Ĵ���
class FileHanleService :
	public IRequestHandle
{
	// ͨ�� IRequestHandle �̳�
	virtual void Process(ProcessArg arg) override;
};

