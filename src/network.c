#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "common.h"
#include "network.h"
#include "crds_pthread.h"
#include "log.h"

int tcpStartup()
{
    int sockfd, clifd, bindRet, listenRet, epfd, fcntlFlags, nfds;
    int i;
    struct sockaddr_in sock;
    struct sockaddr_in cliSock;
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENT];
    socklen_t socklen;

    memset(&sock, 0, sizeof(sock));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    sock.sin_family = AF_INET;
    sock.sin_port = htons(S_BIND_PORT);
#ifdef BIND_IP
    sock.sin_addr.s_addr = inet_addr(BIND_IP);
#else
    sock.sin_addr.s_addr = INADDR_ANY;
#endif

    // bind
    if ((bindRet = bind(sockfd, (struct sockaddr *)&sock, sizeof(sock))) != 0)
    {
        crdsErr("bind failure");
    }

    // listen
    if ((listenRet = listen(sockfd, 128)) != 0)
    {
        crdsErr("listen failure");
    }

    // set fd to Non-blocking
    fcntlFlags = fcntl(sockfd, F_GETFL, 0);
    fcntlFlags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fcntlFlags);

    epfd = epoll_create(MAX_EVENT);

    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
        close(sockfd);
        close(epfd);

        crdsErr("epoll add event failure");
    }

    char buf[1024];
    int index;
    PthreadPool *temp = NULL;

    while (1)
    {
        nfds = epoll_wait(epfd, events, MAX_EVENT, -1);
        if (nfds == -1)
        {
            crdsErr("epoll wait failure");
        }

        for (i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == sockfd)
            {
                socklen = sizeof(cliSock);
                if (clifd = accept(sockfd, (struct sockaddr *)&cliSock, &socklen))
                {
                    index = fdGlobalCount % MAX_EVENT;
                    fdGlobalCount++;

                    // TODO 加锁
                    if (PthreadFdPool[index] == NULL)
                    {
                        PthreadFdPool[index] = (PthreadPool *)malloc(sizeof(PthreadPool));
                    }
                    else
                    {
                        temp = PthreadFdPool[index];
                        PthreadFdPool[index] = (PthreadPool *)malloc(sizeof(PthreadPool));
                        PthreadFdPool[index]->conn = clifd;
                        PthreadFdPool[index]->next = temp;
                        temp = NULL;
                    }
                    // TODO 解锁

                    pthread_mutex_lock(&pcPool[index].mtx);
                    pthread_cond_signal(&pcPool[index].cond);
                    pthread_mutex_unlock(&pcPool[index].mtx);
                }
            }
        }
    }

    return 0;
}

int udpDomainStartup()
{
    return 0;
}

int listenServerStartup()
{
    int sockfd, clifd, bindRet, listenRet, epfd, fcntlFlags, nfds;
    int i;
    struct sockaddr_in sock;
    struct sockaddr_in cliSock;
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENT];
    socklen_t socklen;

    memset(&sock, 0, sizeof(sock));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    sock.sin_family = AF_INET;
    sock.sin_port = htons(L_BIND_PORT);
    sock.sin_addr.s_addr = INADDR_ANY;

    // bind
    if ((bindRet = bind(sockfd, (struct sockaddr *)&sock, sizeof(sock))) != 0)
    {
        crdsErr("bind failure");
    }

    // listen
    if ((listenRet = listen(sockfd, 128)) != 0)
    {
        crdsErr("listen failure");
    }

    // set fd to Non-blocking
    fcntlFlags = fcntl(sockfd, F_GETFL, 0);
    fcntlFlags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fcntlFlags);

    epfd = epoll_create(MAX_EVENT);

    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
        close(sockfd);
        close(epfd);

        crdsErr("epoll add event failure");
    }

    char buf[1024];

    for (i = 0; i < MAX_EVENT; i++)
    {
        pthreadMsg[i] = (char *)malloc(MSG_BUF_LEN);
        bzero(pthreadMsg[i], MSG_BUF_LEN);
    }

    socklen = sizeof(cliSock);
    while(clifd = accept(sockfd, (struct sockaddr *)&cliSock, &socklen))
    {
        while (read(clifd, buf, 1024) > 0)
        {
            for (i = 0; i < MAX_EVENT; i++)
            {
                strcpy(pthreadMsg[i], buf);
                pthread_mutex_lock(&pcPool[i].mtx);
                pthread_cond_signal(&pcPool[i].cond);
                pthread_mutex_unlock(&pcPool[i].mtx);
            }
        }
    }

    return 0;
}
