#pragma once
#include "IRequestHandle.h"
struct  ProcessArg;


//����ģʽ�������һ����Ա�Ĵ���
class HeartBeatService :
	public IRequestHandle
{
	// ͨ�� IRequestHandle �̳�
	virtual void Process(ProcessArg arg) override;
};

