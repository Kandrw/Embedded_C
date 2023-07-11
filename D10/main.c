#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#define RESET//сброс буфера вывода при записи в файл

int main(void){
    pid_t child_pid;
    int status;
    child_pid = fork();
    if(child_pid == 0){
        printf("Child1 pid = %d\n", getpid());
        #ifdef RESET
            fflush(stdout);
        #endif
        child_pid = fork();
        if(child_pid == 0){
            printf("Child4 pid = %d\n", getpid());
            #ifdef RESET
                fflush(stdout);
            #endif
            exit(4);
        }
        child_pid = fork();
        if(child_pid == 0){
            printf("Child5 pid = %d\n", getpid());
            #ifdef RESET
                fflush(stdout);
            #endif
            exit(5);
        }
        wait(&status);
        printf("[2] status = %d\n", WEXITSTATUS(status));
        #ifdef RESET
            fflush(stdout);
        #endif
        wait(&status);
        printf("[2] status = %d\n", WEXITSTATUS(status));
        #ifdef RESET
            fflush(stdout);
        #endif
        exit(2);
    }
    else{
        printf("Parent pid = %d\n", getpid());
        #ifdef RESET
            fflush(stdout);
        #endif
        child_pid = fork();
        if(child_pid == 0){
            printf("Child2 pid = %d\n", getpid());
            #ifdef RESET
                fflush(stdout);
            #endif
            child_pid = fork();
            if(child_pid == 0){
                printf("Child3 pid = %d\n", getpid());
                #ifdef RESET
                    fflush(stdout);
                #endif
                exit(6);
            }
            wait(&status);
            printf("[3] status = %d\n", WEXITSTATUS(status));
            #ifdef RESET
                fflush(stdout);
            #endif
            exit(3);
        }
        wait(&status);
        printf("[1] status = %d\n", WEXITSTATUS(status));
        #ifdef RESET
            fflush(stdout);
        #endif
        wait(&status);
        printf("[1] status = %d\n", WEXITSTATUS(status));
        #ifdef RESET
            fflush(stdout);
        #endif
    }
    printf("END\n");
    return 0;
}


