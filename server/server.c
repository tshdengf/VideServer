#include "work.h"
#include "pthreadpool.h"

int main(int argc, char *argv[])
{
    createlist();
    tpool_create(20);
    signal(SIGPIPE,SIG_IGN);

    int sockfd = Create_sock();
    int epfd;
    struct epoll_event event;
    struct epoll_event events[EVENTS_SIEZ];

    epfd = epoll_create(EVENTS_SIEZ);

    event.events = EPOLLIN;
    event.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    socklen_t c_size;
    struct sockaddr_in c_addr;
    c_size = sizeof(struct sockaddr_in);
    while(1)
    {
        printf("start\n");
        int count = epoll_wait(epfd, events, EVENTS_SIEZ, -1);
        for(int i=0; i<count; i++)
        {
            if(events[i].events == EPOLLIN)
            {
                if(events[i].data.fd == sockfd)
                {
                    long cfd = accept(sockfd,(struct sockaddr*)&c_addr,&c_size);
                    if(cfd < 0)
                    {
                        perror("accept error!");
                        exit(1);
                    }
                    printf("ip = %s port = %d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
                    pthread_t id;
                    // 向线程池添加任务
                    tpool_add_work(thread_read,(void*)cfd);
                }
            }
            
        }
    }
}