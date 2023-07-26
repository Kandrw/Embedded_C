#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>

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
    printf("pid server = %d\n", getpid());
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER, &(addr.sin_addr));
    int fds = socket(AF_INET, SOCK_STREAM, 0);
    if(fds < 0){
        perror("socket");
        return -1;
    }
    int setsock = 1;
    //SO_REUSEPORT 
    if (setsockopt(
        fds, SOL_SOCKET, SO_REUSEADDR, &setsock, sizeof(setsock))) 
        {
        perror("setsockopt");
        return -1;
    }
    if(bind(fds, (struct sockaddr*)&addr, sizeof(addr))){
        printf("errno = %d\n", errno);
        perror("bind");
        return -1;
    }
    listen(fds,1);
    struct sockaddr_in client;
    int size = sizeof(client);
    printf("waiting client...\n");
    int fd2 = accept(fds, (struct sockaddr*)&client, &size);
    printf("fd2 = %d\n");
    char str[INET_ADDRSTRLEN];
    int c_port;
    inet_ntop(AF_INET, &(client.sin_addr), str, INET_ADDRSTRLEN);
    c_port = ntohs(client.sin_port);
    printf("client: %s %d\n", str, c_port);
    if(fd2 == -1){
        perror("accept");
        return -1;
    }
    char buf[256];
    printf("waiting msg...\n");
    if(recv(fd2, buf, sizeof(buf), 0) == -1){
        perror("recv");
        return -1;
    }
    printf("client: %s\n", buf);
    printf("Input: ");
    fgets(buf, sizeof(buf), stdin);
    replace_enter(buf);
    if(send(fd2, buf, strlen(buf), 0) == -1){
        perror("send");
        return -1;
    }
    if(close(fd2)){
        perror("close(client)");
        return -1;
    }
    if(close(fds)){
        perror("close(socket)");
        return -1;
    }
    printf("End\n");
    return 0;
}







