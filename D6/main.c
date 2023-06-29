#include <stdio.h>
#include <stdlib.h>

#include "operations.h"


int main(){


    int command;
    int a, b;
    while(1){
        printf("Command: exit(0), add(1), sub(2), mul(3), div(4) \n");
        printf("Input command: ");
        if( !(scanf("%d", &command) == 1) ){
            command = -1;
            while (getchar() != '\n');
        }
        if(command == 0){
            break;
        }
        printf("Input a b: ");
        if( !(scanf("%d%d", &a, &b) == 2) ){
            printf("Error input arguments\n");
            while (getchar() != '\n');
            continue;
        }
        switch(command){
            case 1:
                printf("\t%d + %d = %d\n", a, b, operation_add(a, b));
                break;
            case 2:
                printf("\t%d - %d = %d\n", a, b, operation_sub(a, b));
                break;
            case 3:
                printf("\t%d * %d = %d\n", a, b, operation_mul(a, b));
                break;
            case 4:
                printf("\t%d / %d = %d\n", a, b, operation_div(a, b));
                break;
            default:
                printf("Command not found\n");
                break;
        }
    }

    printf("Exit\n");
    return 0;
}