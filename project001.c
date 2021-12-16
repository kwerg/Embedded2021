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

#define MY_MESSAGE_ID 8282

void* print_temp(void);               //온도센서에서 온도값 얻어서 fnd에 출력하는 함수 
static pthread_t temp_thread;         //pthread key

typedef struct                        //message structer 
{
	long int messageNum;
	int keyInput;
	int pressed;
} BUTTON_MSG_T2;

static int msgID;

int main(void)
{
    //led & buzzer
    ledLibInit();    //led init
    buzzerInit();   //buzzer init 
    //colorLED
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
    buttonInit();      //이 함수에 msg snd함수 포함되어있습니다. button 눌리는거 차례대로 1,2,3,4,5,6 -> int keyInput 
	pthread_create(&temp_thread, NULL, (void*)(*print_temp), NULL);    //온도출력함수 작동시키는 thread

    while(1)
    {    
        ledtextON();
        lcdtextwrite("Stop",1);   //기차 정차 상태, 정차한 이후  문열림, 문닫힘 상태 구현해야합니다  
        lcdtextwrite("Ready",2); 
        ledtextON();

        pwmSetPercent(0,0);
        pwmSetPercent(80,1);
        pwmSetPercent(0,2);

        while(messageRxData.keyInput != 1) //1번키 = 출발버튼 눌리기 전까지 stop 유지하다가 눌리면 moving으로 상태표시 변환
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, IPC_NOWAIT);
            if(messageRxData.keyInput == 1)
            {  
                pwmSetPercent(0,0);
                pwmSetPercent(0,1);
                pwmSetPercent(80,2);
                lcdtextwrite("",2); 
                lcdtextwrite("Departure",1);
                sleep(1);
                lcdtextwrite("Moving",1);
                ledtextON();
            }
        } 

        int j = 2;      //led 간격 기본 2초로 설정
        int a = 4;      //speed 표시하기 위한 변수
        for(int k=0; k<9; k++)
        {
            ledOnOff(k,1);          //led on
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, IPC_NOWAIT);    //button에서 보낸 신호 받아줌
          //  printf("key = %d, pressed = %d)\r\n",messageRxData.keyInput,messageRxData.pressed);  
            if(messageRxData.keyInput == 2 && j>1)     //현재 2번버튼 = 속도 up 눌리면 led 점등간격 1초 감소
            {  
                j = j-1;
                printf("speed up!  current speed = %d\n\r", a-j);
                lcdtextwrite("speed up!",2); //속도 증가
                ledtextON();
            }
            else if(messageRxData.keyInput == 3 && j <3)  //현재 3번버튼 = 속도 down 눌리면 led 점등간격 1초 증가
            {
                j = j+1;
                printf("speed down!  current speed = %d\n\r", a-j);
                lcdtextwrite("speed down!",2); //속도 감소 
                ledtextON();
            }
            else j = j;

            if(k == 4)  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!도착 임박 알림 음 (추가)
            {
                buzzer_train_arrive();
            }
            messageRxData.keyInput = 0;
            sleep(j); 
            lcdtextwrite("",2); //
            ledOnOff(k,0);        
        }

        lcdtextwrite("Arrived",1);
        ledtextON();
        sleep(1);
        lcdtextwrite("Stop",1);         //기차 정차 상태, 정차한 이후  문열림, 문닫힘 상태 구현해야합니다  
        ledtextON();

        while(1)
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, 0);
            if(messageRxData.keyInput == 4)
            {
                lcdtextwrite("DOOR OPEN",2); 
                ledtextON();
                buzzer_Door_alarm();
                break;
            }
        }

        while(1)
        {
            msgrcv(msgID, &messageRxData, sizeof(messageRxData)-sizeof(long int), 0, 0);
            if(messageRxData.keyInput == 5)
            {
                lcdtextwrite("DOOR CLOSE",2); 
                buzzer_Door_alarm();
                ledtextON();
                lcdtextwrite("Ready",2); 
                ledtextON();
                break;
            }
        }
    }
    pwmStopAll();
    pwmInactiveAll();
    ledLibExit();
    return 0;
}

void* print_temp(void)
{
    while(1)
    {
        double temp = temperature();        //온도센서에서 온도값 얻어서 fnd에 출력하는 함수 
        int temp2 = (int)temp;
        fndDisp(temp2,0);
        sleep(1);
        if(temp2 >= 28)
        {
            lcdtextwrite("Cooling",2); //에어컨 작동
            ledtextON();
            fndDisp(temp2-1,0);
            sleep(1);
            fndDisp(temp2-2,0);
            sleep(1);
        }
        if(temp2 <= 24)
        {
            lcdtextwrite("Heating",2); //히터 가동 
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

void buzzer_Door_alarm(void)                    // Yongin Everline Door open/close alarm
{
    buzzerPlaySong(277); //C# 4 Octave
    usleep(250);  
    buzzerPlaySong(415); //G# 4 Octave
    usleep(500);  
    buzzerPlaySong(554); //C# 5 Octave
    usleep(500);  
    buzzerStopSong();
}

void buzzer_train_arrive(void)                  // Dest.station apporoach alarm (total 8sec)
{
    buzzerPlaySong(262); //C 4 Octave //1s
    usleep(250);  
    buzzerPlaySong(392); //G 4 Octave
    usleep(250);
    buzzerPlaySong(262); //C 4 Octave
    usleep(250);
    buzzerPlaySong(277); //D 4 Octave
    usleep(250);

    buzzerPlaySong(330); //E 4 Octave //2s
    usleep(250);  
    buzzerPlaySong(277); //D 4 Octave
    usleep(250);
    buzzerPlaySong(262); //C 4 Octave
    usleep(250);
    buzzerPlaySong(392); //G 4 Octave
    usleep(250);

    buzzerPlaySong(440); //A 4 Octave //3s
    usleep(250);  
    buzzerPlaySong(349); //F 4 Octave
    usleep(250);
    buzzerPlaySong(440); //A 4 Octave
    usleep(250);
    buzzerPlaySong(262); //C 4 Octave
    usleep(250);

    buzzerPlaySong(349); //F 4 Octave //4s
    usleep(250);  
    buzzerPlaySong(330); //E 4 Octave
    usleep(250);
    buzzerPlaySong(277); //D 4 Octave
    usleep(250);
    buzzerPlaySong(262); //C 4 Octave
    usleep(250);

    buzzerPlaySong(494); //B 4 Octave //5s
    usleep(250);  
    buzzerPlaySong(392); //G 4 Octave
    usleep(250);
    buzzerPlaySong(330); //E 4 Octave
    usleep(250);
    buzzerPlaySong(277); //D 4 Octave
    usleep(250);

    buzzerPlaySong(392); //G 4 Octave //6s
    usleep(250);  
    buzzerPlaySong(349); //F 4 Octave
    usleep(250);
    buzzerPlaySong(330); //E 4 Octave
    usleep(250);
    buzzerPlaySong(277); //D 4 Octave
    usleep(250);

    buzzerPlaySong(330); //E 4 Octave //7s
    usleep(250);  
    buzzerPlaySong(392); //G 4 Octave
    usleep(250);
    buzzerPlaySong(277); //D 4 Octave
    usleep(250);
    buzzerPlaySong(392); //G 4 Octave
    usleep(250);

    buzzerPlaySong(262); //C 4 Octave //8s
    usleep(1000);
    buzzerStopSong();
}