#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "buzzer.h"

#define BUZZER_BASE_SYS_PATH "/sys/bus/platform/devices/"
#define BUZZER_FILENAME "peribuzzer"
#define BUZZER_ENABLE_NAME "enable"
#define BUZZER_FREQUENCY_NAME "frequency"
char gBuzzerBaseSysDir[128]; ///sys/bus/platform/devices/peribuzzer.XX 가 결정됨

int buzzerInit(void)
{
    DIR * dir_info = opendir(BUZZER_BASE_SYS_PATH);
    int ifNotFound = 1;
    if (dir_info != NULL)
    {
        while(1)
        {
            struct dirent *dir_entry;
            dir_entry = readdir (dir_info);
            if (dir_entry == NULL) break;
            if (strncasecmp(BUZZER_FILENAME, dir_entry->d_name, strlen(BUZZER_FILENAME)) == 0)
            {
                ifNotFound = 0;
                sprintf(gBuzzerBaseSysDir,"%s%s/",BUZZER_BASE_SYS_PATH,dir_entry->d_name);
            }
        }

    }
    printf("find %s\n",gBuzzerBaseSysDir);
    return ifNotFound;
}

int buzzerPlaySong(int scale) // 0: 도 , 1:레,....,8:도
{
    int frequency;

    switch(scale)
    {
        case 1: 
            frequency = 262;
            break;

        case 2:
            frequency = 294;
            break;

        case 3:
            frequency = 330;
            break;

        case 4:
            frequency = 349;
            break;

        case 5:
            frequency = 392;
            break;

        case 6:
            frequency = 440;
            break;

        case 7:
            frequency = 494;
            break;
        
        case 8:
            frequency = 523;
            break;  
    }

    char path[200];

    sprintf(path,"%s%s",gBuzzerBaseSysDir,BUZZER_ENABLE_NAME);
    int fd=open(path,O_WRONLY);
    write(fd, &"1", 1);
    close(fd);
    
    sprintf(path,"%s%s",gBuzzerBaseSysDir,BUZZER_FREQUENCY_NAME);
    int fp = open(path,O_WRONLY);
    dprintf(fp, "%d", frequency); 
    close(fp);
}

int buzzerStopSong(void)
{
    char path[200];
    sprintf(path,"%s%s",gBuzzerBaseSysDir,BUZZER_ENABLE_NAME);
    int fd=open(path,O_WRONLY);
    write(fd, &"0", 1);
    close(fd);
}
