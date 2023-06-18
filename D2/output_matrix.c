#include <stdio.h>

#define N 3

int main(){
    int i, j;
    int matrix[N][N];
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            matrix[i][j] = 1 + i * N + j;
        }
    }
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            printf("%2d ", *(matrix[0] + i * N + j));
        }
        printf("\n");
    }
    

    return 0;
}