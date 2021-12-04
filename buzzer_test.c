#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "buzzer.h"

int main()
{
    buzzerInit();
    buzzerEnableDisable(1);

    int i;
    for(i =1; i<9; i++)
    {
        buzzerPlaySong(i);
        sleep(1);
    }   
    buzzerEnableDisable(0);
}