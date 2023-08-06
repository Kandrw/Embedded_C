#include <stdio.h>

int main(){
    int A = 0xDDCCBBAA;
    int byte = 3;
    unsigned char *ptr = (char*)&A;
    printf("byte[%d] = %x\n", byte+1, *(ptr + byte));
    return 0;
}