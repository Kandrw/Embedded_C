#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>//mkfifo
#include <unistd.h>
#include <fcntl.h>//mode_t

int main(){
    printf("Start program1\n");
    char channel_name[] = "channel";
    mkfifo(channel_name, S_IRWXU);
    int fd = open(channel_name, O_RDONLY | O_NONBLOCK);
    int status;
    pid_t pid;
    pid = fork();
    if(pid == 0){
        printf("Test, my pid = %d\n", getpid());
        execl("./program2", "./program2", NULL);
        printf("Error start program2\n");
        close(fd);
        exit(0);
    }
    char buffer[256];
    sleep(1);
    read(fd, buffer, sizeof(buffer));
    printf("message: %s\n", buffer);
    wait(&status);
    close(fd);
    printf("End program1\n");
    return 0;
}


