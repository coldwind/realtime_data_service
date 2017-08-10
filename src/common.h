#ifndef COMMON_HEAD
#define COMMON_HEAD

#include <pthread.h>

#define S_BIND_PORT 12321
#define L_BIND_PORT 12322
#define UD_PATH "../var/crds.sock"
#define NET_TYPE 1
#define MAX_EVENT 128
#define MSG_BUF_LEN 1024

typedef struct pthreadCondPool{
    pthread_cond_t cond;
    pthread_mutex_t mtx;
} PthreadCondPool;

typedef struct pthreadPool {
    int conn;
    struct pthreadPool *next;
} PthreadPool;

extern int fdGlobalCount;
extern PthreadPool *PthreadFdPool[MAX_EVENT];
extern PthreadCondPool pcPool[MAX_EVENT];
extern char *pthreadMsg[MAX_EVENT];

#endif
