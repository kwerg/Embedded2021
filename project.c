#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/msg.h>
#include <pthread.h>
#include "led.h"
#include "button.h" 
#include "textlcddrv.h" 
#include "temperature.h"
#include "fnddrv.h"
#include "buzzer.h"
#include "colorled.h"
#include "jpgAnimation.h"
#define MY_MESSAGE_ID 8282

void* print_temp(void);               //온도센서에서 온도값 얻어서 fnd에 출력하는 함수 
void* print_jpg(void);            
static pthread_t temp_thread;         //pthread key
static pthread_t jpg_thread;   

typedef struct                        //message structer 
{
	long int messageNum;
	int keyInput;
	int pressed;
} BUTTON_MSG_T2;

static int msgID;

int main(void)
{
    //init
    ledLibInit();    //led init
    buzzerInit();
    buttonInit();   //이 함수에 msg snd함수 포함되어있습니다. button 눌리는거 차례대로 1,2,3,4,5,6 -> int keyInput 
    
    pwmStopAll();
    pwmActiveAll();

    pwmSetDuty(0,0);
    pwmSetDuty(0,1);
    pwmSetDuty(0,2);

    pwmSetPeriod(1000000,0);
    pwmSetPeriod(1000000,1);
    pwmSetPeriod(1000000,2);

    pwmStartAll();

    //button
    BUTTON_MSG_T2 messageRxData;
    messageRxData.messageNum = 1;                  
    msgID = msgget((key_t)MY_MESSAGE_ID, IPC_CREAT|0666);        //msg ID생성
	pthread_create(&temp_thread, NULL, (void*)(*print_temp), NULL);    //온도출력함수 작동시키는 thread
	pthread_create(&jpg_thread, NULL, (void*)(*print_jpg), NULL);    //tft lcd 작동시키는 thread
    while(1)
    {    
        ledtextON();
        lcdtextwrite("Stop",1);         //기차 정차 상태 
        lcdtextwrite("Ready",2);        //출발 가능 상태
        ledtextON();

        pwmSetPercent(0,0);
        pwmSetPercent(80,1);
        pwmSetPercent(0,2);

        while(messageRxData.keyInput != 1) //1번버튼 눌리지 않으면 출발 X
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, IPC_NOWAIT);
            if(messageRxData.keyInput == 1)   //1번버튼 눌렸을때
            {  
                pwmSetPercent(0,0);
                pwmSetPercent(0,1);
                pwmSetPercent(80,2);          //color led -> red
                lcdtextwrite("Departure",1);  //1초간 txt lcd 1번째칸에 출발 표시
                sleep(1);
                lcdtextwrite("Moving",1);    //열차 이동중일때 Moving 표시
                lcdtextwrite("",2); 
                ledtextON();
            }
        }       
        int j = 2;             //led 간격 기본 2초로 설정 -> led 8개이므로 총 이동시간 default value = 16초로 표현
        int a = 4;             //speed 표시하기 위한 변수
        for(int k=0; k<9; k++)
        {
            ledOnOff(k,1);          //led on
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, IPC_NOWAIT); //button에서 보낸 신호 받아줌
            if(messageRxData.keyInput == 2 && j>1)     //현재 2번버튼 = 속도 up 눌리면 led 점등간격 1초 감소 (속도 증가)
            {  
                j = j-1;
                printf("speed up!  current speed = %d\n\r", a-j);
                lcdtextwrite("speed up!",2);    //다음 led로 넘어가기전까지 txt lcd 2번째칸에 속도 증가 표시
                ledtextON();
            }
            else if(messageRxData.keyInput == 3 && j <3)  //현재 3번버튼 = 속도 down 눌리면 led 점등간격 1초 증가 (속도 감소)
            {
                j = j+1;
                printf("speed down!  current speed = %d\n\r", a-j);
                lcdtextwrite("speed down!",2);  //다음 led로 넘어가기전까지 txt lcd 2번째칸에 속도 감소 표시
                ledtextON();
            }
            else j = j;
            messageRxData.keyInput = 0;
            sleep(j);                            //led 점등간격
            lcdtextwrite("",2);     //speed 증가, 감소 표시 꺼줌
            ledOnOff(k,0);       //led off 
        } 
        lcdtextwrite("Arrived",1);     //led가 처음부터 끝 led까지 점등을 모두 마치면 txt lcd 1번째칸에 도착 표시 (1초간)
        ledtextON();
        sleep(1);
        lcdtextwrite("Stop",1);         //txt lcd 1번째칸에 기차 정차 상태 표시
        ledtextON();

        while(1)    //4번키(문열림) 눌릴 때까지 기다림
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, 0);
            if(messageRxData.keyInput == 4)    //4번키 눌리면         
            {
                lcdtextwrite("DOOR OPEN",2);     //txt lcd 2번째칸에 문열림 표시
                ledtextON();
                buzzerPlaySong(440);     //버저 1초간울림
                sleep(1);  
                buzzerStopSong(); 
                break;
            }
        }

        while(1)     //4번키(문닫힘) 눌릴 때까지 기다림
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, 0);
            if(messageRxData.keyInput == 5)
            {
                lcdtextwrite("DOOR CLOSE",2); //txt lcd 2번째칸에 문열림 표시
                ledtextON();
                buzzerPlaySong(440);  //버저 1초간울림
                sleep(1); 
                lcdtextwrite("Ready",2); //txt lcd 2번째칸에 출발가능상태 표시  
                ledtextON();
                buzzerStopSong(); 
                break;
            }
        }
    }
    pwmStopAll();
    pwmInactiveAll();
    ledLibExit();
    return 0;
}

void* print_temp(void)     /*온도 센서 함수 -> 28도 이상넘어가면 fnd에 표시되는 온도가 26도로 내려가며 txt-lcd 2번째칸에 cooling(냉방중) 표시
                                                 24도 이상 떨어지면 fnd에 표시되는 온도가 26도로 올라가며 txt-lcd 2번째칸에 heating(난방중) 표시 */ 
{
    while(1)
    {
        double temp = temperature();        //온도센서에서 온도값 얻어서 fnd에 출력하는 함수 
        int temp2 = (int)temp;
        fndDisp(temp2,0);
        sleep(1);
        if(temp2 >= 28)
        {
            lcdtextwrite("Cooling",2); 
            ledtextON();
            fndDisp(temp2-1,0);
            sleep(1);
            fndDisp(temp2-2,0);
            sleep(1);
        }
        if(temp2 <= 24)
        {
            lcdtextwrite("Heating",2); 
            ledtextON();
            fndDisp(temp2+1,0);
            sleep(1);
            fndDisp(temp2+2,0);
            sleep(1);
        }
        lcdtextwrite("",2); 
        ledtextON();
    }
    return 0;
}

void* print_jpg(void)     //tft-lcd 함수 1번키 눌리기 전까지 animation 작동 x, 작동끝나고 문열림, 문닫힘 버튼 순서대로 들어오기 전까지 다른 활동 x
{
    BUTTON_MSG_T2 messageRxData;
    messageRxData.messageNum = 1;

    while(1)
    { 
        while(messageRxData.keyInput != 1) //1번키 = 출발버튼 눌리기 전까지 stop 유지하다가 눌리면 moving으로 상태표시 
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, IPC_NOWAIT);
        }  
        jpgAnimation();
        while(messageRxData.keyInput != 4)  
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, IPC_NOWAIT);
        }  
        while(messageRxData.keyInput != 5)  
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, IPC_NOWAIT);
        }  
    }
}


    
