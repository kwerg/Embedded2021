#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <pthread.h> 
#include <sys/msg.h>


// first read input device
#include "button.h"
#define INPUT_DEVICE_LIST "/dev/input/event"
//실제 디바이스 드라이버 노드파일: 뒤에 숫자가 붙음., ex)/dev/input/event5
#define PROBE_FILE "/proc/bus/input/devices"
//PPT에 제시된 "이 파일을 까보면 event? 의 숫자를 알수 있다"는 바로 그 파일

int main(int argc, char *argv[])
{
    BUTTON_MSG_T messageRxData;
    buttonInit();    
    int msgID = msgget(MESSAGE_ID, IPC_CREAT|0666);
    msgrcv(msgID, &messageRxData, sizeof(messageRxData.keyInput) + sizeof(messageRxData.pressed), 0, 0);
    printf ("keyInput: %d pressed: %d\r\n",messageRxData.keyInput, messageRxData.pressed);
    return 1;
}



