#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/poll.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define PORT_SERVER 39999
#define PORT 24444
#define SIZE_HEADER 20
#define SIZE_H_UDP 8
#define SIZEBUF 200
#define COUNT_ATTEMPT 20

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
    struct sockaddr_in client;
    char buffer[SIZEBUF + SIZE_H_UDP];
    char msg_server[SIZEBUF];
    struct pollfd fd_poll;
    int res;
    int i_count_att = 0;
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    memset((char*)&client, 0, sizeof(struct sockaddr_in));
    if(fd < 0){
        perror("socket");
        return -1;
    }
    int size = sizeof(struct sockaddr_in);
    struct udphdr header;
    memset((char*)&header, 0, SIZE_H_UDP);
    fd_poll.fd = fd;
    fd_poll.events = POLLIN;
    while(1){
        printf("Input: ");
        fgets(buffer+SIZE_H_UDP, SIZEBUF-SIZE_H_UDP, stdin);
        replace_enter(buffer+SIZE_H_UDP);
        if(!strcmp(buffer+SIZE_H_UDP, "exit")){
            break;
        }
        header.source = htons(PORT);
        header.dest = htons(PORT_SERVER);
        header.len = htons(SIZEBUF);
        header.check = 0;
        for(int i = 0; i < SIZE_H_UDP; ++i){
            buffer[i] = *((char*)&header + i);
        }
        #ifdef DEBUG
            printf("send\n");
            for(int i = 0; i < SIZEBUF + SIZE_H_UDP; ++i){
                printf("%d ", (unsigned char)buffer[i]);
            }
            printf("\n");
        #endif
        if(sendto(fd, buffer, SIZEBUF+SIZE_H_UDP, 0, (struct sockaddr*)&client, size) == -1){
            perror("sendto");
        }
        i_count_att = 0;
        while(i_count_att < COUNT_ATTEMPT){
            res = poll(&fd_poll, 1, 100);
            if(res == -1){
                perror("poll");
            }
            else if(res > 0){
                if(recvfrom(fd, msg_server, SIZEBUF, 0, NULL, NULL) == -1){
                    perror("recvform");
                }
                strncpy((char*)&header, msg_server + SIZE_HEADER, SIZE_H_UDP);
                #ifdef DEBUG
                    printf("recv:\n");
                    for(int i = 0; i < SIZEBUF; ++i){
                        printf("%d ", (unsigned char)msg_server[i]);
                    }
                    printf("\n");
                    printf("%d -> %d\n", ntohs(header.source), ntohs(header.dest));
                #endif
                if(ntohs(header.source) == PORT_SERVER && ntohs(header.dest) == PORT)
                    printf("msg server: %s\n", msg_server + SIZE_HEADER + SIZE_H_UDP);
            }
            i_count_att++;
        }
    }
    if(close(fd)){
        perror("close");
    }

    printf("End client\n");
    return 0;
}

