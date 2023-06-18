#include <stdio.h>

#define N 20

int main(){
    volatile int i, i2, buffer;

    int array[N];
    for(i = 0; i < N; ++i){
        array[i] = i+1;
    }
    for(i = 0; i < N; ++i){
        printf("%d ", array[i]);
    }
    printf("\n");
    for(i = 0, i2 = N - 1; i < N/2; ++i, --i2){
        buffer = array[i];
        array[i] = array[i2];
        array[i2] = buffer;
    }
    for(i = 0; i < N; ++i){
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}