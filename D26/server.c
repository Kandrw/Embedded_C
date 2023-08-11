#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <signal.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
    argiments:
        ADDRESS
        PORT
*/

#define SIZEBUF 200

void *thread_server(void *args){
    int fd = *(int*)args;
    char buffer[SIZEBUF + 10];
    char msg_client[SIZEBUF];
    char str[INET_ADDRSTRLEN];
    int count_package = 0;
    struct sockaddr_in client;
    int size = sizeof(struct sockaddr_in);
    memset((char*)&client, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, ADDRESS, &client.sin_addr);
    client.sin_port = htons(PORT);
    client.sin_family = AF_INET;
    
    printf("Start server: fd:%d, %s:%d\n", fd, ADDRESS, PORT);
    
    while(1){
        memset(msg_client, 0, SIZEBUF);
        if(recvfrom(fd, msg_client, SIZEBUF, 0, (struct sockaddr*)&client, (socklen_t*)&size) == -1){
            perror("recvform");
        }
        inet_ntop(AF_INET, &client.sin_addr, str, INET_ADDRSTRLEN);
        
        if(ntohs(client.sin_port) != PORT && (strcmp(str, ADDRESS) != 0)){
            count_package++;
            printf("count packege = %d\n", count_package);
            printf("[%s:%d] ", str, ntohs(client.sin_port));
            printf("client: %s\n", msg_client);
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer,"server-%s", msg_client);
            
            if(sendto(fd, buffer, SIZEBUF, 0, (struct sockaddr*)&client, (socklen_t)size) == -1){
                perror("sendto");
            }
        }
        else{
            #ifdef DEBUG
                printf("[%s:%d] ", str, ntohs(client.sin_port));
                printf("msg: %s\n", msg_client);
            #endif
        }
    }
    return NULL;
}
int main(){

    struct sockaddr_in server;
    memset((char*)&server, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, ADDRESS, &server.sin_addr);
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    if(bind(fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) ){
        perror("bind");
        return -1;
    }
	
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
    printf("End server\n");
    return 0;
}
