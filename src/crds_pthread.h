#define PTHREAD_NUM 4

void *workerStartup(void *);
int workerCleanup();

void *listenStartup(void *);
