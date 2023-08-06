#include <stdio.h>

#define N 20

void print_matrix(int M[N][N]){
    int i, j;
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            printf("%3d ", *(M[0] + i * N + j));
        }
        printf("\n");
    }
}

int main(){
    int matrix[N][N];
    int i, j;
    int i2, j2;
    int n = N, n2 = N-1, n3 = N - 1, n4 = N-2;
    int iter = 1;
    /*Test*/
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            matrix[i][j] = -1;
        }
    }
    int c = 1;
    int c2 = 1;
    int k1 = 0, k2 = 0;
    int k3 = 1, k4 = N-1;
    int k5 = N-2, k6 = N-1;
    int k7 = N-2, k8 = 0;
    for(iter = 1; iter <= N * N;){
            for(j = k2; j < n; ++j){
                matrix[k1][j] = iter;
                iter++;
            }
            k1++;
            k2++;
            n--;

            for(i = k3; i <= n2; ++i){
                matrix[i][k4] = iter;
                iter++;
            }
            k3++;
            k4--;
            n2--;

            for(j = k5; j > k5-n3; --j){
                matrix[k6][j] = iter;
                iter++;
            }
            k5--;
            k6--;
            n3-=2;

            for(i = k7; i > k7 - n4; --i){
                matrix[i][k8] = iter;
                iter++;
            }
            k7--;
            k8++;
            n4-=2;

    }
    print_matrix(matrix);    
    printf("End\n");
    return 0;
}