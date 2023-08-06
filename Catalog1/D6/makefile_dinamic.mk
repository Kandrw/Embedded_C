
.PHONY: lib main.o main
all: lib main.o main clear

lib:
	gcc -fPIC -c operation_add.c 
	gcc -fPIC -c operation_sub.c 
	gcc -fPIC -c operation_mul.c 
	gcc -fPIC -c operation_div.c 
	gcc -shared operation_add.o operation_sub.o operation_mul.o operation_div.o -o liboperations.so
main.o:
	gcc -c main.c -o main.o
main:
	-rm calculator
	gcc -o calculator main.o -L. -loperations
#	запуск	
#	make -f makefile_dinamic.mk
#	LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH ./calculator
clear:
	-rm *.o

