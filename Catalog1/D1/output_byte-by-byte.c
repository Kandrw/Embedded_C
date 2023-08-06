#include <stdio.h>


int main(){

    int a = 0xDDCCBBAA;
    printf("a = %X\n", a);
    unsigned char iter = 0xFF;
    int i = 24;
    while(0 <= i){
        printf("byte[%d] = %X\n", i/8, (a >> i) & iter);
        i -= 8;
    }
    return 0;
}