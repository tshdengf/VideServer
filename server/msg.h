#ifndef _MSG_H_
#define _MSG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <signal.h>

#include <pthread.h>
#include <sqlite3.h>
#include <unistd.h>


struct online
{
    int cfd;
    char name[20];
    struct online *next;
    // struct online *pre;
};

typedef struct messgae
{
    int flag;
    int action;
    char name[20];
    char password[20];
    char toname[20];
    char filename[20];
    char message[1024];
    struct online* head;
}Msg;



#endif // !_MSG_H_
