#pragma once
#include "IRequestHandle.h"
#include "ProcessArg.h"


//����ģʽ�������һ����Ա�Ĵ���
class LoginService :
	public IRequestHandle
{
	// ͨ�� IRequestHandle �̳�
	virtual void Process(ProcessArg arg) override;
};

