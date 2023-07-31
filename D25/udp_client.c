#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <signal.h>

#include <sys/poll.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER "127.0.0.1"
#define PORT 29999
#define SIZEBUF 256
#define SIZE_HEADER 20
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
    memset((char*)&client, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, SERVER, &client.sin_addr);
    client.sin_port = htons(PORT);
    client.sin_family = AF_INET;
    int size = sizeof(struct sockaddr_in);
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    char buffer[SIZEBUF];
    char msg_server[SIZEBUF];
    struct pollfd fd_poll;
    int res;
    if(fd < 0){
        perror("socket");
        return -1;
    }
    if(bind(fd, (struct sockaddr*)&client, sizeof(struct sockaddr_in)) ){
        perror("bind");
        return -1;
    }
    fd_poll.fd = fd;
    fd_poll.events = POLLIN;
    printf("Init socket - %d\n", fd);
    
    int i_count_att = 0;
    while(1){
        printf("Input: ");
        fgets(buffer, SIZEBUF, stdin);
        replace_enter(buffer);
        if(!strcmp(buffer, "exit")){
            break;
        }
        #ifdef DEBUG
            printf("size_msg = %d\n", strlen(buffer));
            for(int i = 0; i < strlen(buffer); ++i){
                
                printf("%d ", buffer[i]);
            }
            printf("\n");
        #endif
        if(sendto(fd, buffer, SIZEBUF, 0, (struct sockaddr*)&client, size) == -1){
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
                if(strcmp(msg_server+SIZE_HEADER, buffer)){
                    #ifdef DEBUG
                        printf("size_msg server = %d\n", strlen(msg_server+SIZE_HEADER));
                    #endif
                    printf("server: %s\n", msg_server + SIZE_HEADER);
                }
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