#ifndef _BUZZER_H_
#define _BUZZER_H_
int buzzerInit(void);
int buzzerPlaySong(int frequency);
int buzzerStopSong(void);
int buzzerExit(void);
void buzzer_Door_alarm(void);
#endif