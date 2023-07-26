#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

#include <sys/socket.h>

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
    char filename[40];
    if(argc > 1){
        strcpy(filename, argv[1]);
    }
    else{
        strcpy(filename, SERVERNAME);
    }
    int fds = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fds < 0){
        perror("socket");
        return -1;
    }
    struct sockaddr addr = {AF_LOCAL, 0};
    strcpy(addr.sa_data, filename);
    if(connect(fds, &addr, sizeof(struct sockaddr))){
        perror("connect");
        return -1;
    }
    char buf[30];
    printf("Input: ");
    fgets(buf, sizeof(buf), stdin);
    replace_enter(buf);
    send(fds, buf, sizeof(buf), 0);
    recv(fds, buf, sizeof(buf), 0);
    printf("server: %s\n", buf);
    if(close(fds) == -1){
        perror("close(socket)");
        return -1;
    }
    printf("End\n");
    return 0;
}







