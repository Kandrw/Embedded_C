#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER "127.0.0.1"
#define PORT 4567

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
int main(){
    int fds = socket(AF_INET, SOCK_DGRAM, 0);
    if(fds < 0){
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER, &(addr.sin_addr));
    if(connect(fds, (struct sockaddr*)&addr, sizeof(addr))){
        perror("connect");
        return -1;
    }
    char buf[256];
    printf("Input: ");
    fgets(buf, sizeof(buf), stdin);
    replace_enter(buf);
    if(send(fds, buf, sizeof(buf), 0) == -1){
        perror("send");
        return -1;
    }
    printf("waiting msg...\n");
    if(recv(fds, buf, sizeof(buf), 0) == -1){
        perror("recv");
        return -1;
    }
    printf("server: %s\n", buf);
    if(close(fds) == -1){
        perror("close(socket)");
        return -1;
    }
    printf("End client\n");
    return 0;
}







