#pragma once
#include <string.h>
//协议 头文件
#define MSG_HEAD_LEN  sizeof(MsgHeader)
#define MSG_HEADER  "EVEN"

#define HEART_CHECK   0x10

#define USER_LOGIN 0x20

#define UPLOAD_FILE 0x30

#define DELETE_CACHE_FILE 0x31

#define UPLOAD_FILE_OVER 0x32

#define GET_VIDEO 0x40



//负责处理消息的类（类似于标志位）

//消息头
struct MsgHeader
{
	//消息头
	unsigned char msgHeader[4];
	//请求的操作 如登录，注册等
	unsigned short controlMask;
	//消息头 附带正文内容的长度
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

//登录信息 消息正文
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
	bool success;               //是否成功
	unsigned char errorMsg[1024];   //错误信息
	int reserved;       //保留位
};

//上传文件结构体
struct FileUpLoadInfo
{
	char fileName[1024];		//文件名
	char md5[32];				//文件MD5
	char buffer[1024];			//文件流信息
	int fileLen;			    //文件大小
	unsigned short bufferLen;	//文件流Buffer大小
	int  seekIndex;				//文件发送结束标志

};

struct RequestReplay
{
	char msg[1024];
	unsigned short code;
};