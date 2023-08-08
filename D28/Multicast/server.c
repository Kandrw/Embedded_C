#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZEBUF 200

enum pos_argv{
    LOCADDR = 1,
    MULTIADDR,
    PORT,

};
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
    int dest_port;
    if(argc < 4){
        printf("Not enough arguments: locaddr, multi address, dest port\n");
        return -1;
    }
    if(sscanf(argv[PORT], "%d", &dest_port) != 1){
        printf("Error: incorrect dest port %s\n", argv[PORT]);
        return -1;
    }
    if(dest_port < 1025 || dest_port > 0xFFFF){
        printf("Error port");
        return -1;
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    char buffer[SIZEBUF];
    struct sockaddr_in client;
    memset((void*)&client, 0, sizeof(client));
    client.sin_port = htons(dest_port);
    client.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[MULTIADDR], &client.sin_addr) <= 0){
        printf("Error: incorrect dest address %s\n", argv[MULTIADDR]);
        close(fd);
        return -1;
    }
    struct in_addr localaddr;
    if(inet_pton(AF_INET, argv[LOCADDR], &localaddr) <= 0){
        printf("Error: incorrect local address %s\n", argv[LOCADDR]);
        close(fd);
        return -1;
    }
    if(setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localaddr, sizeof(localaddr))){
        perror("setsockopt");
        close(fd);
        return -1;
    }
    while(1){
        printf("Input: ");
        memset(buffer, 0, SIZEBUF);
        fgets(buffer, SIZEBUF, stdin);
        replace_enter(buffer);
        if(!strcmp(buffer, "exit")){
            break;
        }
        if(sendto(fd, buffer, SIZEBUF, 0, (struct sockaddr*)&client, sizeof(client)) == -1){
            perror("sendto");
            break;
        }
    }
    if(close(fd)){
        perror("close");
    }
    printf("End server\n");
    return 0;
}
