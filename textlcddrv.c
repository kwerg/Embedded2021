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

#define LINE_NUM			2
#define COLUMN_NUM			16
#define TEXLCD_DRIVER_NAME      "/dev/peritextlcd"

int lcdtextwrite(const char *str1,int lineFlag)
{
    stTextLCD stlcd;
    int fd;
    int len;
    unsigned int linenum = 0;

    linenum = lineFlag;
    memset(&stlcd, 0, sizeof(stTextLCD)); // 초기화
    if(linenum == 1)
        stlcd.cmdData = CMD_DATA_WRITE_LINE_1;
    else if (linenum == 2)
        stlcd.cmdData = CMD_DATA_WRITE_LINE_2;
    else
    {
        perror("line 설정 에러: 1또는 2로 설정 바람\n");
        return 1;
    }

    len = strlen(str1);
    if (len > COLUMN_NUM)
        memcpy(stlcd.TextData[stlcd.cmdData-1],str1,COLUMN_NUM);
    else   
        memcpy(stlcd.TextData[stlcd.cmdData-1],str1,len);

    stlcd.cmd = CMD_WRITE_STRING;
    fd = open(TEXLCD_DRIVER_NAME,O_RDWR); // driver open
    if(fd<0){
        perror("driver open error!!!\n");
        return 1;
    }

    write(fd,&stlcd,sizeof(stTextLCD));
    close(fd);
    return 0;
}

int ledtextON()
{
    stTextLCD stlcd;
    int fd;

    stlcd.cmd = CMD_DISPLAY_MODE;
    stlcd.cmdData = BIT_DISPLAY_MODE_DISP_ENABLE;
    fd = open(TEXLCD_DRIVER_NAME,O_RDWR); // driver open
    if(fd<0){
        perror("driver open error!!!\n");
        return 1;
    }

    write(fd,&stlcd,sizeof(stTextLCD));
    close(fd);
    return 0;
    
}
