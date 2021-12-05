#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "textlcddrv.h"

int main()
{
    ledtextON();
    lcdtextwrite("Hello Linux!!",1);
    ledtextON();
}