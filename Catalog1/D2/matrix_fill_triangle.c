#include <stdio.h>

#define N 4

void print_matrix(int M[N][N]){
    int i, j;
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            printf("%d ", *(M[0] + i * N + j));
        }
        printf("\n");
    }
}

int main(){
    int matrix[N][N];
    int i, j;
    int i2, j2;
    int n = N, n2 = 0;
    for(i = 0, i2 = N - 1; i < N; ++i, --i2){
        for(j = 0, j2 = n2; j < n; ++j, ++j2){
            if(j < n-1)
                matrix[i][j] = 0;
            matrix[i2][j2] = 1;
        }
        --n;
        ++n2;
    }
    print_matrix(matrix);    
    return 0;
}