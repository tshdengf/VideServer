#pragma once

struct  ProcessArg;

//����ģʽ�б�����Ļ�Ա
//����ͻ������� ������
class IRequestHandle
{
public:
	virtual void Process(ProcessArg arg) = 0;
};

