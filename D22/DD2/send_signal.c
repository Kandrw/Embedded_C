#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int main(){
    int sig;
    int pid;
    printf("Input pid: ");
    if(scanf("%d", &pid) <= 0){
        perror("scanf pid");
        return -1;
    }
    if(pid <= 1000){
        printf("Error pid\n");
        return 0;
    }
    while(1){
        printf("Input signal(0 - exit): ");
        if(scanf("%d", &sig) <= 0){
            while(getchar() != '\n');
            printf("Error input sig\n");
            continue;
        }
        if(sig == 0)
            break;
        if(kill((pid_t)pid, sig) == -1){
            perror("kill");
            if(ESRCH == errno || EPERM == errno)
                exit(0);
            continue;
        }
        printf("sig = %d\n", sig);
    }
    printf("End\n");
    return 0;
}