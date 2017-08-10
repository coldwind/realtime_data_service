#include <stdio.h>
#include <stdlib.h>

int crdsErr(char *msg)
{
    printf("crds-error:%s\n", msg);
    exit(1);
}

int crdsWarning(char *msg)
{
    printf("crds-warning:%s\n", msg);
}

int crdsNotice(char *msg)
{
    printf("crds-notice:%s\n", msg);
}
