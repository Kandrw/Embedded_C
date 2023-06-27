
.PHONY: main main.o 
all: main.o main clear start 

main:
	-rm Program
	gcc -o Program main.o

main.o:
	gcc -c main.c -g -o main.o 

start:
	-./Program
clear:
	-rm *.o

