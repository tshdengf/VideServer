#pragma once

int SetNonBlock(int iSock);


void SyncNoBlockServer();

int Send(int sockfd, const char *buf, int sendSize);

int Recv(int sockfd, char *buf, int recvSize);

void HandlerNoBlck(int fd);

