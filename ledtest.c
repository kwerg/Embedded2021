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

#include "led.h"
#define LED_DRIVER_NAME "/dev/periled"

int main()
{
    int Num , status;
    ledLibInit();

    while(1)
    {
        printf("lED제어 상태 입력:(LED번호/ON or OFF)\n");
        scanf("%d %d",&Num,&status);
        ledOnOff(Num,status);
    }
    
    ledLibExit();

}
