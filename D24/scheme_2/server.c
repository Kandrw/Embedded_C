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
#define PORT 20001
#define SIZEBUF 256
#define COUNT_SERVER_DEFAULT 50

#define SIZE_MSG_CHANNEL sizeof(int)

struct Server_serv{
    struct sockaddr_in server;
    struct Server_serv *next;
    int fd;
    pthread_t id;
    int run;
    int index;
};

struct Server_serv **list_serv;

void sigusr1(int sig, siginfo_t *siginf_pr, void *ptr){
    //BLOCKED
}
void sigusr2(int sig, siginfo_t *siginf_pr, void *ptr){
    //BLOCKED
}
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
void *server_serving_dinamic(void *argv);
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
    int i;
    while(1){
        
        new_fd = accept(serv->fd, (struct sockaddr*)&client, &size);
        if(new_fd < -1){
            perror("accept");
            continue;
        }
        for(i = 0; i < COUNT_SERVER_DEFAULT; ++i){
            if(!list_serv[i]->run){
                break;
            }
        }
        if(i < COUNT_SERVER_DEFAULT){
            list_serv[i]->run = 1;
            list_serv[i]->fd = new_fd;
            pthread_kill(list_serv[i]->id, SIGUSR1);
            continue;
        }
        end_list->next = Create_Server_serv();
        end_list->next->fd = new_fd;
        if(pthread_create(&end_list->next->id, NULL, server_serving_dinamic, (void**)end_list->next) == -1){
            perror("pthread_create");
            Delete_Server_serv(end_list->next);
            continue;
        }
        end_list = end_list->next;
    }
    return NULL;
}


void *server_serving_dinamic(void *argv){
    struct Server_serv *serv = (struct Server_serv*)argv;
    char buffer[SIZEBUF];
    send(serv->fd, buffer, SIZEBUF, 0);
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
    printf("End serving server: %d\n", (int)serv->id);
    return NULL;
}

void *server_serving_static(void *argv){
    struct Server_serv *serv = (struct Server_serv*)argv;
    char buffer[SIZEBUF];
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("ERROR sigemptyset");
        return NULL;
    }
    sigaddset(&signalSet, SIGUSR1);
    sigaddset(&signalSet, SIGUSR2);
    sigaddset(&signalSet, SIGPIPE);
    pthread_sigmask(SIG_SETMASK, &signalSet, NULL);
    
    int sig;
    while(1){
        printf("[%d] wait\n",serv->index);
        sigwait(&signalSet, &sig);
        if(sig == SIGPIPE){
            printf("[%d] disconnect client\n", serv->index);
            continue;
        }
        if(sig == SIGUSR2){
            break;
        }
        #ifdef DEBUG
            printf("[%d] sig = %d, start\n",serv->index, sig);
        #endif
        sprintf(buffer, "connect");
        #ifdef DEBUG
            printf("[%d] Connect\n", serv->index);
        #endif
        if(send(serv->fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
            serv->run = 0;
            continue;
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
        #ifdef DEBUG
            printf("[%d] close fd\n", serv->index);
        #endif
        serv->run = 0;
        
    }
    printf("End serving server: %d\n", (int)serv->index);
    return NULL;
}

int main(){
    printf("pid = %d\n", getpid());
    printf("Start server2\n");

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
    printf("Create server listen\n");
    struct sigaction act = { 0 };
    act.sa_sigaction = sigusr1;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        return -1;
    }
    struct sigaction act2 = { 0 };
    act2.sa_sigaction = sigusr2;
    act2.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR2, &act2, NULL) == -1) {
        perror("sigaction2");
        return -1;
    }
    #ifdef DEBUG
        printf("Create sigusr1, sigusr2\n");
    #endif
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
    #ifdef DEBUG
        printf("Create list ignore signal\n");
    #endif
    list_serv = (struct Server_serv**)malloc(sizeof(struct Server_serv*) * COUNT_SERVER_DEFAULT);
    for(int i = 0; i < COUNT_SERVER_DEFAULT; ++i){
        list_serv[i] = Create_Server_serv();
        list_serv[i]->run = 0;
        list_serv[i]->index = i;
        pthread_create(&list_serv[i]->id, NULL, server_serving_static, (void**)list_serv[i]);
    }
    #ifdef DEBUG
        printf("Create pull thread\n");
    #endif
    int setsock = 1;
    if (setsockopt(head->fd, SOL_SOCKET, SO_REUSEADDR, &setsock, sizeof(setsock))){
        perror("setsockopt");
        return -1;
    }
    pthread_create(&head->id, NULL, server_listen, (void**)head);
    int sig;
    while(1){
        sigwait(&signalSet, &sig);
        printf("sig = %d\n", sig);
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT){
            break;
        }
    }
    #ifdef DEBUG
        printf("Deleting\n");
    #endif
    struct Server_serv *ptr = head;
    head = head->next;
    close(ptr->fd);
    free(ptr);
    while(head){
        ptr = head;
        head = head->next;
        if(ptr->fd != -1){
            if(close(ptr->fd)){
                #ifdef DEBUG
                    perror("close server");
                #endif
            }
        }
        free(ptr);
    }
    for(int i = 0; i < COUNT_SERVER_DEFAULT; ++i){
        pthread_kill(list_serv[i]->id, SIGUSR2);
        close(list_serv[i]->fd);
    }
    printf("End server2\n");
}



