#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include "common.h"
#include "crds_pthread.h"
#include "log.h"

pthread_t workers[PTHREAD_NUM];
int connPool[PTHREAD_NUM];

int workerInit()
{
    printf("start\n");
    int i;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    printf("num:%d\n", PTHREAD_NUM);

    for (i = 0; i < PTHREAD_NUM; i++)
    {
        int arg = i;

        pthread_cond_init(&pcPool[i].cond, NULL);
        pthread_mutex_init(&pcPool[i].mtx, NULL);

        if (pthread_create(&workers[i], NULL, workerStartup, &arg) != 0)
        {
            crdsErr("pthread startup failure");
        }

        PthreadFdPool[i] = NULL;
    }
}

void *workerStartup(void *arg)
{
    int index = *(int *)arg;
    struct timespec outtime;
    struct timeval now;
    struct epoll_event event;

    int epfd = epoll_create(MAX_EVENT);

    //epoll_ctl();

    pthread_mutex_lock(&pcPool[index].mtx);

    while (1)
    {
        gettimeofday(&now, NULL);
        outtime.tv_sec = now.tv_sec + 2;
        outtime.tv_nsec = 0;
        pthread_cond_timedwait(&pcPool[index].cond, &pcPool[index].mtx, &outtime);

        // insert clifd
        if (PthreadFdPool[index] != NULL)
        {
            // TODO 加锁

            // 加入epoll fd

        }

        // push msg
        if (strlen(pthreadMsg[index]) > 0)
        {
            printf("%s\n", pthreadMsg[index]);
            //bzero(pthreadMsg[index], MSG_BUF_LEN);
        }

        //if ()
        //{
        //}
        // check worker list
        // add fd
    }

    pthread_mutex_unlock(&pcPool[index].mtx);
}

void *listenStartup(void *arg)
{
    listenServerStartup();
}

int workerCleanup()
{
}
