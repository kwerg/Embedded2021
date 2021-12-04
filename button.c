#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/msg.h>
#include <pthread.h>
#include "button.h"
// first read input device
#define INPUT_DEVICE_LIST "/dev/input/event" //실제 디바이스 드라이버 노드파일: 뒤에 숫자가 붙음., ex)/dev/input/event5
#define PROBE_FILE "/proc/bus/input/devices" //PPT에 제시된 "이 파일을 까보면 event? 의 숫자를 알수 있다"는 바로 그 파일
#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"
#define MY_MESSAGE_ID 8282

static pthread_t buttonTh_id;
static int fd;
static int msgID;
static BUTTON_MSG_T messageTxData;

int probeButtonPath(char *newPath)
{
	int returnValue = 0; //button에 해당하는 event#을 찾았나?
	int number = 0; //찾았다면 여기에 집어넣자
	FILE *fp = fopen(PROBE_FILE,"rt"); //파일을 열고
	
	while(!feof(fp)) //끝까지 읽어들인다.
	{
		char tmpStr[200]; //200자를 읽을 수 있게 버퍼
		fgets(tmpStr,200,fp); //최대 200자를 읽어봄
		//printf ("%s",tmpStr);
		if (strcmp(tmpStr,HAVE_TO_FIND_1) == 0)
		{
			printf("YES! I found!: %s\r\n", tmpStr);
			returnValue = 1; //찾음
		}
		if (
		(returnValue == 1) && //찾은 상태에서
		(strncasecmp(tmpStr, HAVE_TO_FIND_2,
		strlen(HAVE_TO_FIND_2)) == 0) //Event??을 찾았으면
		)
		{
			printf ("-->%s",tmpStr);
			printf("\t%c\r\n",tmpStr[strlen(tmpStr)-3]);
			number = tmpStr[strlen(tmpStr)-3] - '0';
			//Ascii character '0'-'9' (0x30-0x39)
			//to interger(0)
			break;
		}
	}
	fclose(fp);
	if (returnValue == 1)
	sprintf (newPath,"%s%d",INPUT_DEVICE_LIST,number);
	return returnValue;
}
	
void* buttonThFunc(void)
{
    int readSize,inputIndex;
    struct input_event stEvent;
    BUTTON_MSG_T messageTxData;
    messageTxData.messageNum = 1;
    int msgID = msgget((key_t)MY_MESSAGE_ID, IPC_CREAT|0666);

    while(1)
    {
        readSize = read(fd, &stEvent , sizeof(stEvent));              
        if (readSize != sizeof(stEvent))
        {
            continue;
        }
        if ( stEvent.type == EV_KEY)
        {
            printf("EV_KEY(");
            switch(stEvent.code)
            {
                case KEY_VOLUMEUP: messageTxData.keyInput = 5; break;
                case KEY_HOME: messageTxData.keyInput = 1; break;
                case KEY_SEARCH: messageTxData.keyInput = 3; break;
                case KEY_BACK: messageTxData.keyInput = 2; break;
                case KEY_MENU: messageTxData.keyInput = 4; break;
                case KEY_VOLUMEDOWN: messageTxData.keyInput = 6; break;
            }
            if ( stEvent.value ) messageTxData.pressed = 1;
            else messageTxData.pressed = 0;
            msgsnd(msgID, &messageTxData, sizeof(messageTxData)-sizeof(long int), 0);
        } //End of if
        else // EV_SYN
            ; // do notthing
    } // End of While
        close(fd);
}
 

int buttonInit()
{  	
	char buttonPath[200]={0,};	
	if ( probeButtonPath(buttonPath) == 0)
	{
		printf ("ERROR! File Not Found!\r\n");
		printf ("Did you insmod?\r\n");
		return 0;
	}
	fd=open(buttonPath, O_RDONLY);
	if(fd == -1)
        printf("open error\r\n");    
	else
        printf("open success\r\n");
	
	int err = pthread_create(&buttonTh_id, NULL, (void*)(*buttonThFunc), NULL);
	if(err == 0)
        printf("pthread_create success\r\n");    
	else
        printf("pthread_create failed\r\n");    

	pthread_detach(buttonTh_id);
	return 1;
}
