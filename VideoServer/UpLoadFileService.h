#pragma once
#include "IRequestHandle.h"
#include "ProcessArg.h"

//����ģʽ�������һ����Ա�Ĵ���
class UpLoadFileService :
	public IRequestHandle
{
	// ͨ�� IRequestHandle �̳�
	virtual void Process(ProcessArg arg) override;
};

