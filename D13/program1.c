#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main(){
    
    printf("Start program1\n");
    int fides[2];
    pipe(fides);
    printf("open channel: read - %d, write - %d\n", fides[0], fides[1]);
    int status;
    pid_t pid;
    pid = fork();
    if(pid == 0){
        printf("Test, my pid = %d\n", getpid());
        dup2(fides[0], 0);
        execl("./program2", "./program2", NULL);
        printf("Error start program2\n");
        exit(0);
    }
    char mes[256] = "test message";
    write(fides[1], mes, strlen(mes));
    
    wait(&status);
    close(fides[0]);
    close(fides[1]);

    printf("End program1\n");

}


