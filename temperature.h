#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

int spi_init(char[]);
char * spi_read_lm74(int);
double temperature(void);

#endif
