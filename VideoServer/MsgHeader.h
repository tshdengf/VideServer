#pragma once
#include <string.h>
//Э�� ͷ�ļ�
#define MSG_HEAD_LEN  sizeof(MsgHeader)
#define MSG_HEADER  "EVEN"

#define HEART_CHECK   0x10

#define USER_LOGIN 0x20

#define UPLOAD_FILE 0x30

#define DELETE_CACHE_FILE 0x31

#define UPLOAD_FILE_OVER 0x32

#define GET_VIDEO 0x40



//��������Ϣ���ࣨ�����ڱ�־λ��

//��Ϣͷ
struct MsgHeader
{
	//��Ϣͷ
	unsigned char msgHeader[4];
	//����Ĳ��� ���¼��ע���
	unsigned short controlMask;
	//��Ϣͷ �����������ݵĳ���
	int contentLength;

	MsgHeader(int cLength, unsigned short cMask)
	{
		strcpy((char *)msgHeader, MSG_HEADER);
		contentLength = cLength;
		controlMask = cMask;
	}
	MsgHeader()
	{
		strcpy((char *)msgHeader, MSG_HEADER);
	}
};

//��¼��Ϣ ��Ϣ����
struct LoginInfo
{
	unsigned char username[35];
	unsigned char password[35];
	unsigned char ip[35];
	int reserved;

	LoginInfo(char * ip, char * name, char *passMD5)
	{
		strcpy((char*)this->ip, ip);
		strcpy((char *)this->username, name);
		strcpy((char *)this->password, passMD5);
	}
	LoginInfo()
	{

	}
};

struct VerifyLoginInfo
{
	bool success;               //�Ƿ�ɹ�
	unsigned char errorMsg[1024];   //������Ϣ
	int reserved;       //����λ
};

//�ϴ��ļ��ṹ��
struct FileUpLoadInfo
{
	char fileName[1024];		//�ļ���
	char md5[32];				//�ļ�MD5
	char buffer[1024];			//�ļ�����Ϣ
	int fileLen;			    //�ļ���С
	unsigned short bufferLen;	//�ļ���Buffer��С
	int  seekIndex;				//�ļ����ͽ�����־

};

struct RequestReplay
{
	char msg[1024];
	unsigned short code;
};