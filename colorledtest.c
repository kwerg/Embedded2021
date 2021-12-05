#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define COLOR_LED_DEV_R_ "/sys/class/pwm/pwmchip0/"
#define COLOR_LED_DEV_G_ "/sys/class/pwm/pwmchip1/"
#define COLOR_LED_DEV_B_ "/sys/class/pwm/pwmchip2/"
#define PWM_EXPORT "export"
#define PWM_UNEXPORT "unexport"
#define PWM_DUTY "pwm0/duty_cycle"
#define PWM_PERIOD "pwm0/period"
#define PWM_ENABLE "pwm0/enable"
#define PWM_COLOR_R 0
#define PWM_COLOR_G 1
#define PWM_COLOR_B 2
#define PWM_PERIOD_NS 1000000 //ns. = 1ms = 1khz 
#include "colorled.h"

int main(void)
{
    int a = 0;
    int b = 1;
    int c = 2;
    int d = 80;
    int e = 1000000;
    pwmStopAll();
    sleep(3);
    pwmActiveAll();

    pwmSetDuty(0,0);
    pwmSetDuty(0,1);
    pwmSetDuty(0,2);

    pwmSetPeriod(1000000,0);
    pwmSetPeriod(1000000,1);
    pwmSetPeriod(1000000,2);

    pwmStartAll();

    pwmSetPercent(80,0);
    pwmSetPercent(0,1);
    pwmSetPercent(0,2);
    sleep(2);
    pwmSetPercent(0,0);
    pwmSetPercent(80,1);
    pwmSetPercent(0,2);
    sleep(2);  
    pwmSetPercent(0,0);
    pwmSetPercent(0,1);
    pwmSetPercent(80,2);
    sleep(2);

    pwmStopAll();
    pwmInactiveAll();
   
    return 0;
}
