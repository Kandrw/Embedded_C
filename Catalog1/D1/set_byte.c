#include <stdio.h>


int main(){

    int a = 0xDDCCBBAA;
    unsigned char byte_index = 2;//3 байт
    unsigned char byte_new = 0x11;
    a &= ~(0xFF << (byte_index * 8));//зануление нужного байта
    printf("a = %x\n", a);
    a |= byte_new << (byte_index * 8);//новое значение в нужный байт
    printf("a = %x\n", a);


    return 0;
}