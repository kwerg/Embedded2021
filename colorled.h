#ifndef _COLORLED_H_
#define _COLORLED_H_

int pwmActiveAll(void);
int pwmInactiveAll(void);
int pwmSetDuty(int, int);
int pwmSetPeriod(int, int);
int pwmSetPercent(int, int);
int pwmStartAll(void);
int pwmStopAll(void);

#endif
