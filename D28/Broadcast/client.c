#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <sys/poll.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZEBUF 200

int port;
char *address;
int run = 1;

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
void *thread_recvfrom(void *args){
    int fd = *(int*)args;
    char buffer[SIZEBUF];
    struct sockaddr_in server;
    int size = sizeof(server);
    int res;
    struct pollfd fd_poll;
    fd_poll.fd = fd;
    fd_poll.events = POLLIN;
    while(run){
        res = poll(&fd_poll, 1, 100);
        if(res == -1){
            perror("poll");
        }
        else if(res > 0){
            if(recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server, (socklen_t*)&size) == -1){
                perror("recvfrom");
            }
            printf("msg: %s\n", buffer);
        }
    }
    return NULL;
}


int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Not enough arguments: source address, source port\n");
        return -1;
    }
    if(sscanf(argv[2], "%d", &port) != 1){
        printf("Error: incorrect source port %s\n", argv[2]);
        return -1;
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    printf("Init socket - %d\n", fd);

    struct sockaddr_in client;
    memset((char*)&client, 0, sizeof(struct sockaddr_in));
    if(inet_pton(AF_INET, argv[1], &client.sin_addr) <= 0){
        printf("Error: incorrect source address %s\n", argv[1]);
        close(fd);
        return -1;
    }
    client.sin_port = htons(port);
    client.sin_family = AF_INET;
    
    if(bind(fd, (struct sockaddr*)&client, sizeof(struct sockaddr_in)) ){
        perror("bind");
        return -1;
    }
    char buffer[SIZEBUF];
    pthread_t id;
    pthread_create(&id, NULL, thread_recvfrom, (void**)&fd);

    while(1){
        printf("Input: ");
        fgets(buffer, SIZEBUF, stdin);
        replace_enter(buffer);
        if(!strcmp(buffer, "exit")){
            run = 0;
            break;
        }
    }
    int status;
    pthread_join(id, (void**)&status);
    if(close(fd)){
        perror("close");
    }

    printf("End client\n");
    return 0;
}