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
    int fds = socket(AF_INET, SOCK_DGRAM, 0);
    if(fds < 0){
        perror("socket");
        return -1;
    }
    int setsock = 1;
    if (setsockopt(fds, SOL_SOCKET, SO_REUSEADDR, &setsock, sizeof(setsock))){
        perror("setsockopt");
        return -1;
    }
    if(bind(fds, (struct sockaddr*)&addr, sizeof(addr))){
        printf("errno = %d\n", errno);
        perror("bind");
        return -1;
    }
    char buffer[256];
    struct sockaddr_in client;
    int size = sizeof(client);
    printf("waitnig client...\n");
    if(recvfrom(fds, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &size) == -1){
        perror("recvfrom");
        return -1;
    }
    char str[INET_ADDRSTRLEN];
    int c_port;
    inet_ntop(AF_INET, &(client.sin_addr), str, INET_ADDRSTRLEN);
    c_port = ntohs(client.sin_port);
    printf("client(%s:%d): %s\n", str, c_port, buffer);
    size = sizeof(client);
    printf("Input: ");
    fgets(buffer, sizeof(buffer), stdin);
    replace_enter(buffer);
    if(sendto(fds, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, size) == -1){
        printf("errno = %d\n", errno);
        perror("sendto");
        return -1;
    }
    if(close(fds)){
        perror("close(socket)");
        return -1;
    }
    printf("End server\n");
    return 0;

}







