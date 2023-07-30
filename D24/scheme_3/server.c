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
#define PORT 20002
#define SIZEBUF 256

#define SIZE_MSG_CHANNEL sizeof(int)

#define SIZE_MSG_CHANNEL_LS sizeof(struct Server_serv *)

struct Server_serv{
    struct sockaddr_in server;
    struct Server_serv *next;
    int fd;
    pthread_t id;
    int run;
};
int fides[2];
int fides_list_serv[2];
pthread_t id_free_serv;


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
void *server_listen(void *argv){
    struct Server_serv *serv = (struct Server_serv*)argv;
    listen(serv->fd, 5);
    struct sockaddr_in client;
    int size = sizeof(client);
    int new_fd;
    while(1){
        new_fd = accept(serv->fd, (struct sockaddr*)&client, &size);
        if(new_fd < -1){
            perror("accept");
            continue;
        }
        
        write(fides[1], (char*)&new_fd, SIZE_MSG_CHANNEL);
    }
    return NULL;
}

void *server_serving(void *argv){
    struct Server_serv *serv = (struct Server_serv*)argv;
    char buffer[SIZEBUF];
    int run = 1;
    if(send(serv->fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
        close(serv->fd);
        run = 0;
    }
    while(run){
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
    serv->run = 0;
    pthread_kill(id_free_serv, SIGUSR1);
    return NULL;
}

void *generate_server_serv(void *argv){
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("generate_server_serv: ERROR sigemptyset");
        return NULL;
    }
    sigaddset(&signalSet, SIGPIPE);
    pthread_sigmask(SIG_SETMASK, &signalSet, NULL);
    
    int fd;
    int res;
    struct Server_serv *new_serv;
    while(1){
        res = read(fides[0], (char*)&fd, SIZE_MSG_CHANNEL);
        if(fd < 0 || res < SIZE_MSG_CHANNEL){
            printf("Error\n");
            continue;
        }
        new_serv = Create_Server_serv();
        new_serv->fd = fd;
        new_serv->run = 1;
        #ifdef DEBUG
            printf("Create serv\n");
        #endif
        if(pthread_create(&new_serv->id, NULL, server_serving, (void**)new_serv) == -1){
            perror("pthread_create");
            Delete_Server_serv(new_serv);
            continue;
        }
        write(fides_list_serv[1], (char*)&new_serv, SIZE_MSG_CHANNEL_LS);
        #ifdef DEBUG
            printf("write serv\n");
        #endif
    }
    return NULL;
}

void *free_serv_in_channel(void *argv){
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("free_serv_in_channel: ERROR sigemptyset");
        return NULL;
    }
    sigaddset(&signalSet, SIGUSR1);
    sigaddset(&signalSet, SIGPIPE);
    sigaddset(&signalSet, SIGTERM);
    pthread_sigmask(SIG_SETMASK, &signalSet, NULL);
    struct Server_serv *new_serv;
    int count_servfree = 0;
    int sig;
    while(1){
        sigwait(&signalSet, &sig);
        if(sig == SIGPIPE){
            printf("Error: SIGPIPE\n");
            break;
        }
        if(sig == SIGTERM){
            break;
        }
        if(sig == SIGUSR1){
            while(1){
                if(read(fides_list_serv[0], (char*)&new_serv, SIZE_MSG_CHANNEL_LS) < SIZE_MSG_CHANNEL_LS){
                    sleep(1);
                    printf("Error read\n");
                    continue;
                }
                if(!new_serv){
                    printf("not found ptr\n");
                    sleep(1);
                    continue;
                }
                if(new_serv->run){
                    write(fides_list_serv[1], (char*)&new_serv, SIZE_MSG_CHANNEL_LS);
                    continue;
                }
                printf("Delete serv[%d]\n", new_serv->id);
                free(new_serv);
                count_servfree++;
                #ifdef DEBUG
                    printf("count free serv = %d\n", count_servfree);
                #endif
                break;
            }
        }
        else{
            #ifdef DEBUG
                printf("not found sig: %d\n", sig);
            #endif
        }
    }
    return NULL;
}   

int main(){
    printf("pid = %d\n", getpid());
    printf("Start server3\n");
    pipe(fides);
    pipe(fides_list_serv);
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
    #ifdef DEBUG
        printf("bind server listen\n");
    #endif
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
        printf("Create sigusr(1,2)\n");
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
    int setsock = 1;
    if (setsockopt(head->fd, SOL_SOCKET, SO_REUSEADDR, &setsock, sizeof(setsock))){
        perror("setsockopt");
        return -1;
    }
    pthread_create(&head->id, NULL, server_listen, (void**)head);
    #ifdef DEBUG
        printf("Create: server listen\n");    
    #endif
    
    pthread_t id_gen;
    pthread_create(&id_gen, NULL, generate_server_serv, (void**)NULL);
    #ifdef DEBUG
        printf("Create: generate_server_serv\n");
    #endif
    pthread_create(&id_free_serv, NULL, free_serv_in_channel, (void**)NULL);
    #ifdef DEBUG
        printf("Create: free_serv_in_channel\n");
    #endif
    
    int sig;
    while(1){
        sigwait(&signalSet, &sig);
        #ifdef DEBUG
            printf("sig = %d\n", sig);
        #endif
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT){
            break;
        }
    }
    #ifdef DEBUG
        printf("Clear resourse\n");
    #endif
    pthread_kill(id_free_serv, SIGTERM);
    close(head->fd);
    free(head);
    close(fides[0]);  close(fides[1]);
    close(fides_list_serv[0]); close(fides_list_serv[1]);
    
    printf("End server3\n");
}



