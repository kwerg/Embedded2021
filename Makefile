all: final.elf

final.elf: 1.o 2.o main.c
	gcc -o final.elf main.c 1.o 2.o 

1.o: 1.c myProject.h
	gcc 1.c -c

2.o: 2.c myProject.h
	gcc 2.c -c 

main.o: main.c myProject.h
	gcc main.c

clean: 
	rm -rf 1.o 2.o main.o final.elf