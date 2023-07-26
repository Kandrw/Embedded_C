#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

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
    int fds = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(fds < 0){
        perror("socket");
        return -1;
    }
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, filename);
    if(connect(fds, (struct sockaddr*)&addr, sizeof(addr))){
        perror("connect");
        return -1;
    }
    char buf[256];
    printf("Input: ");
    fgets(buf, sizeof(buf), stdin);
    replace_enter(buf);
    send(fds, buf, sizeof(buf), 0);
    if(close(fds) == -1){
        perror("close(socket)");
        return -1;
    }
    printf("End\n");
    return 0;
}







