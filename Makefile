all: project.elf

project.elf: project.o libMyPeri.a
	arm-linux-gnueabi-gcc project.o -l MyPeri -L. -o project.elf -lpthread

project.o: project.c led.h button.h buzzer.h fnddrv.h textlcddrv.h colorled.h temperature.h
	arm-linux-gnueabi-gcc --static -c project.c -o project.o

libMyPeri.a: led.o button.o buzzer.o fnddrv.o textlcddrv.o colorled.o temperature.o  
	arm-linux-gnueabi-ar rc libMyPeri.a led.o button.o buzzer.o fnddrv.o textlcddrv.o colorled.o temperature.o 

led.o: led.c led.h
	arm-linux-gnueabi-gcc --static -c led.c -o led.o

button.o: button.c button.h
	arm-linux-gnueabi-gcc --static -c button.c -o button.o

buzzer.o: buzzer.c buzzer.h
	arm-linux-gnueabi-gcc --static -c buzzer.c -o buzzer.o

fnddrv.o: fnddrv.c fnddrv.h
	arm-linux-gnueabi-gcc --static -c fnddrv.c -o fnddrv.o

textlcddrv.o: textlcddrv.c textlcddrv.h
	arm-linux-gnueabi-gcc --static -c textlcddrv.c -o textlcddrv.o

colorled.o: colorled.c colorled.h
	arm-linux-gnueabi-gcc --static -c colorled.c -o colorled.o

temperature.o: temperature.c temperature.h
	arm-linux-gnueabi-gcc --static -c temperature.c -o temperature.o

clean: 
	rm -rf project.elf libMyPeri.a led.o button.o buzzer.o fnddrv.o textlcddrv.o colorled.o temperature.o
	
