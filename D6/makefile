
.PHONY: lib main.o main
all: lib main.o main 


lib:
	gcc -c operation_add.c 
	gcc -c operation_sub.c 
	gcc -c operation_mul.c 
	gcc -c operation_div.c 
	ar rc liboperations.a operation_add.o operation_sub.o operation_mul.o operation_div.o

main.o:
	gcc -c main.c -o main.o

main:
	-rm calculator
	gcc -o calculator main.o -L. -loperations

start:
	./calculator
clear:
	-rm *.o
	-rm *.a

