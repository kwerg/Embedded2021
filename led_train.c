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

#include 'led.h'
#define LED_DRIVER_NAME "/dev/periled"

int main()
{
   ledLibInit();
   printf("Train Depart!!!\n");

   int k = 1;
   for(k = 1; k < 9;k++)
   {
       ledOnOff(k,1);
       delay(2000);
       ledOnOff(k,0);
   }
   printf("Train Arrived!!!\n");
   ledLibExit();

}