#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <sys/poll.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZEBUF 200

enum pos_argv{
    LOCADDR = 1,
    MULTIADDR,
    PORT,
};

int port;
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
            if(recv(fd, buffer, SIZEBUF, 0) == -1){
                perror("recv");
            }
            printf("msg: %s\n", buffer);
        }
    }
    return NULL;
}


int main(int argc, char *argv[]){
    if(argc < 4){
        printf("Not enough arguments\n");
        return -1;
    }
    if(sscanf(argv[PORT], "%d", &port) != 1){
        printf("Error: incorrect source port %s\n", argv[PORT]);
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
    if(inet_pton(AF_INET, argv[MULTIADDR], &client.sin_addr) <= 0){
        printf("Error: incorrect source address %s\n", argv[MULTIADDR]);
        close(fd);
        return -1;
    }
    client.sin_port = htons(port);
    client.sin_family = AF_INET;
    struct ip_mreqn multicast;
    memset((void*)&multicast, 0, sizeof(multicast));
    multicast.imr_ifindex = 0;
    if(inet_pton(AF_INET, argv[MULTIADDR], &multicast.imr_multiaddr) <= 0){
        perror("multiaddr, inet_pton");
        close(fd);
        return -1;
    }
    if(inet_pton(AF_INET, argv[LOCADDR], &multicast.imr_address) <= 0){
        perror("locaddr, inet_pton");
        close(fd);
        return -1;
    }
    if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast, sizeof(multicast))){
        printf("errno = %d\n", errno);
        perror("setsockopt");
        close(fd);
        return -1;
    }
    
    if(bind(fd, (struct sockaddr*)&client, sizeof(struct sockaddr_in)) ){
        perror("bind");
        close(fd);
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