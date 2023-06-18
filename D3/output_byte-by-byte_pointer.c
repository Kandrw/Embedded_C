#include <stdio.h>

int main(){
    int A = 0xDDCCBBAA;
    int i;
    unsigned char *ptr = (char*)&A;
    printf("A = 0d%d = 0x%X\n", A, A);
    for(i = sizeof(int) - 1; i >= 0; --i){
        printf("%x\n", *(ptr+i));
    }
    return 0;
}