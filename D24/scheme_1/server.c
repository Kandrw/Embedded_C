#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include <unistd.h> 
#include <string.h>
#include <errno.h>

#include <pthread.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER "127.0.0.1"
#define PORT 20000
#define SIZEBUF 256


struct Server_serv{
    struct sockaddr_in server;
    struct Server_serv *next;
    int fd;
    pthread_t id;
};

struct Server_serv *Create_Server_serv(){
    struct Server_serv *new_elem = (struct Server_serv*)malloc(sizeof(struct Server_serv));
    new_elem->next = NULL;
    return new_elem;
}

int Delete_Server_serv(struct Server_serv *serv){
    if(!serv){
        return -1;
    }
    close(serv->fd);
    free(serv);
    return 0;
}
void *server_serving(void *argv);


void *server_listen(void *argv){
    struct Server_serv *serv = (struct Server_serv*)argv;
    listen(serv->fd, 5);
    struct sockaddr_in client;
    int size = sizeof(client);
    int new_fd;
    char buffer[SIZEBUF];
    memset(buffer, 0, SIZEBUF);
    struct Server_serv *end_list = serv;
    int count_client = 0;
    while(1){
        new_fd = accept(serv->fd, (struct sockaddr*)&client, &size);
        if(new_fd < -1){
            perror("accept");
            continue;
        }
        end_list->next = Create_Server_serv();
        end_list->next->fd = new_fd;
        if(pthread_create(&end_list->next->id, NULL, server_serving, (void**)end_list->next) == -1){
            perror("pthread_create");
            Delete_Server_serv(end_list->next);
            continue;
        }
        end_list = end_list->next;
        count_client++;
        printf("count client = %d\n", count_client);
    }
    
    return NULL;
}

void *server_serving(void *argv){
    struct Server_serv *serv = (struct Server_serv*)argv;
    char buffer[SIZEBUF];
    sprintf(buffer, "connect\n");
    if(send(serv->fd, buffer, strlen(buffer), 0) == -1){
        perror("send");
    }
    while(1){
        if(recv(serv->fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
            close(serv->fd);
            break;
        }
        sprintf(buffer, "msg received");
        if(send(serv->fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
            close(serv->fd);
            break;
        }
    }
    serv->fd = -1;
    printf("End serving server: %d\n", (int)serv->id);
    return NULL;
}

int main(){
    printf("pid = %d\n", getpid());
    printf("Start server1\n");

    struct Server_serv *head = Create_Server_serv();
    inet_pton(AF_INET, SERVER, &(head->server.sin_addr));
    head->server.sin_family = AF_INET;
    head->server.sin_port = htons(PORT);
    head->fd = socket(AF_INET, SOCK_STREAM, 0);
    if(head->fd < 0){
        perror("socket");
        free(head);
        return -1;
    }
    if( bind(head->fd, (struct sockaddr*)&head->server, sizeof(struct sockaddr_in)) ){
        perror("bind");
        free(head);
        return -1;
    }
    int setsock = 1;
    if (setsockopt(
        head->fd, SOL_SOCKET, SO_REUSEADDR, &setsock, sizeof(setsock))) 
        {
        perror("setsockopt");
        return -1;
    }
    int *status;
    if(!head){
        printf("Exit\n");
        return -1;
    }
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
    pthread_create(&head->id, NULL, server_listen, (void**)head);
    int sig;
    while(1){
        sigwait(&signalSet, &sig);
        printf("sig = %d\n", sig);
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT){
            break;
        }
    }
    struct Server_serv *ptr;
    while(head){
        ptr = head;
        head = head->next;
        if(ptr->fd != -1){
            if(close(ptr->fd)){
                perror("close server");
            }
            printf("close server %d\n", ptr->fd);
        }
        free(ptr);
    }
    
    printf("End server1\n");
}



