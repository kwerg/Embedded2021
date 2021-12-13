#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "fnddrv.h"

int main()
{
    int i;
    for(i=0;i < 1000;i++)
    {
        fndDisp(i,0);
        sleep(1);
    }
}