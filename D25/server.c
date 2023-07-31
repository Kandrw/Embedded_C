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
#define SIZE_H_UDP 8

void *thread_server(void *args){
    int fd = *(int*)args;
    char buffer[SIZEBUF];
    char msg_client[SIZEBUF];
    
    struct sockaddr_in client;
    int size = sizeof(struct sockaddr_in);
    int res;
    int count_package = 0;
    memset((char*)&client, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, SERVER, &client.sin_addr);
    client.sin_port = htons(PORT);
    client.sin_family = AF_INET;
    printf("Start server: %d\n", fd);
    while(1){
        memset(msg_client, 0, SIZEBUF);
        if(recvfrom(fd, msg_client, SIZEBUF, 0, NULL, NULL) == -1){
            perror("recvform");
        }
        #ifdef DEBUG
            printf("size_msg = %d\n", strlen(msg_client+SIZE_HEADER));
        #endif
        if(strcmp(buffer, msg_client+SIZE_HEADER)){
            count_package++;
            printf("count packege = %d\n", count_package);
            #ifdef DEBUG
                for(int i = 0; i < SIZEBUF; ++i){
                    printf("%c", msg_client[i]);
                }
                printf("\n");
                for(int i = 0; i < SIZEBUF; ++i){
                    
                    printf("%d ", msg_client[i]);
                }
                printf("\n");
            #endif
            printf("msg: %s\n", msg_client+SIZE_HEADER+SIZE_H_UDP);
            sprintf(buffer, "1234%s", msg_client+SIZE_HEADER+SIZE_H_UDP);
            if(sendto(fd, buffer, SIZEBUF, 0, (struct sockaddr*)&client, size) == -1){
                perror("sendto");
            }
        }
    }
    return NULL;
}

int main(){

    struct sockaddr_in server;
    memset((char*)&server, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, SERVER, &server.sin_addr);
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP | IPPROTO_ICMP);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    if(bind(fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) ){
        perror("bind");
        return -1;
    }
    printf("Init socket - %d\n", fd);

    pthread_t id_server;

    pthread_create(&id_server, NULL, thread_server, (void**)&fd);
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("sigemptyset");
        return -1;
    }
    if(sigfillset(&signalSet)){
        perror("sigfillset");
        return -1;
    }
    sigprocmask(SIG_SETMASK, &signalSet, NULL);
    int sig;
    while(1){
        sigwait(&signalSet, &sig);
        printf("sig = %d\n", sig);
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT){
            break;
        }
    }
    if(close(fd)){
        perror("close");
    }
    printf("End\n");
    return 0;
}



