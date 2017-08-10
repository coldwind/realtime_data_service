#include <stdio.h>
#include "common.h"
#include "network.h"
#include "crds_pthread.h"

int fdGlobalCount = 0;

PthreadCondPool pcPool[MAX_EVENT];
PthreadPool *PthreadFdPool[MAX_EVENT];
char *pthreadMsg[MAX_EVENT];

int main(int argc, char *argv[])
{
    // pthread startup
    workerInit();
    listenServerStartup();
    //while (1)
    //{
    //}

    // network startup
    tcpStartup();

    return 0;
}
