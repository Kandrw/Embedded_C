#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <errno.h>

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
    printf("pid server = %d\n", getpid());
    struct sockaddr_un addr = {AF_LOCAL, SERVERNAME};
    int fds = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(fds < 0){
        perror("socket");
        return -1;
    }
    printf("fds = %d\n", fds);
    unlink(SERVERNAME);
    if(bind(fds, (struct sockaddr*)&addr, sizeof(addr))){
        printf("errno = %d\n", errno);
        perror("bind");
        return -1;
    }
    char buffer[256];
    struct sockaddr_un client;
    int size = sizeof(client);
    printf("waitnig...\n");
    int res = recvfrom(fds, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &size);
    printf("client: %s\n", buffer);
    if(close(fds)){
        perror("close(socket)");
        return -1;
    }
    remove(SERVERNAME);
    printf("End\n");
    return 0;
}
