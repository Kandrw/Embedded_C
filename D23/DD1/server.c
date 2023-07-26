#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h> 

#define SERVERNAME "SERVER1"

void replace_enter(char *str){
    int i = 0;
    while('\0' != str[i]){
        if(str[i] == '\n'){
            str[i] = '\0';
            break;
        }
        ++i;
    }
}

int main(int argc, char *argv[]){
    printf("pid server = %d\n", getpid());
    struct sockaddr addr = {AF_LOCAL, SERVERNAME};
    printf("s = %s\n", addr.sa_data);
    int fds = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fds < 0){
        perror("socket");
        return -1;
    }
    if(bind(fds, &addr, sizeof(struct sockaddr))){
        printf("errno = %d\n", errno);
        perror("bind");
        return -1;
    }
    listen(fds,1);
    struct sockaddr client;
    int size = sizeof(client);
    int fd2 = accept(fds, &client, &size);
    char buf[30];
    recv(fd2, buf, sizeof(buf), 0);
    printf("client: %s\n", buf);
    printf("Input: ");
    fgets(buf, sizeof(buf), stdin);
    replace_enter(buf);
    send(fd2, buf, strlen(buf), 0);
    close(fd2);
    if(close(fds)){
        perror("close(socket)");
        return -1;
    }
    remove(SERVERNAME);
    printf("End\n");
    return 0;
}







