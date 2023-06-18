#include <stdio.h>

#define N 5

void print_matrix(int M[N][N]){
    int i, j;
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            printf("%2d ", *(M[0] + i * N + j));
        }
        printf("\n");
    }
}

int main(){
    int matrix[N][N];
    int i, j;
    int i2, j2;
    int n = N, n2 = 0;
    int iter = 1;
    for(i = 0; i < N; ++i){
        for(j = 0; j < n; ++j){
            //matrix[i][j] = iter;
            //iter++;
        }
    }
    n = N;
    int c = 1;
    int c2 = 1;
    int k1 = 0, k2 = 0;
    int k3 = 0, k4 = 0;
    
    for(iter = 1; iter <= N * N;){
        
        if(c2 == 1){
            for(i = k1, j = k2; i < n; ++j){
                matrix[i][j] = iter;
                iter++;
            }
            ++k1;
            for(i = k1, j = k2; i < n; ++j){
                matrix[i][j] = iter;
                iter++;
            }
        }
        else{
            iter++;
        }
        c--;

        if(c == 0){
            c = 2;
            if(c2 == 1){
                c2 = 0;
            }
            else{
                c2 = 1;
            }
            //n--;
        }
    }
    print_matrix(matrix);    
    return 0;
}