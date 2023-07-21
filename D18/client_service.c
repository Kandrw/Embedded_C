#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <pthread.h>

#include "struct_message.h"

char login[SIZE_MSG];

void *thread_receive_msg(void *args){
    struct msg_buf msg;
    int msgqueue_key = *(int*)args;
    printf("start: thread_receive_msg %d\n", msgqueue_key);
    while(1){
        if(msgrcv( msgqueue_key, &msg, SIZE_STRUCT_MSG, TYPE_SENDING_SERVICE_MSG, 0) == -1){
            printf("Error: receiving msg\n");
            return NULL;
        }
        printf("message: %s\n", msg.mtext);
    }
    return NULL;
}
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
int init_msg_send(int msgqueue_key){
    struct msg_buf msg;
    msg.mtype = TYPE_INIT;
    msg.init = 1;
    memset(msg.mtext, 0, SIZE_MSG);
    sprintf(msg.mtext, "%d", rand());
    printf("Login: @%d\n", msg.mtext);
    strcpy(login, msg.mtext);
    if(msgsnd(msgqueue_key, &msg, SIZE_STRUCT_MSG, 0) == -1){
        perror("init_msg_send: ");
        return -1;
    }
    return 0;
}
int exit_msg_send(int msgqueue_key){
    struct msg_buf msg;
    msg.mtype = TYPE_INIT;
    msg.init = 2;
    memset(msg.mtext, 0, SIZE_MSG);
    strcpy(msg.mtext, login);
    if(msgsnd(msgqueue_key, &msg, SIZE_STRUCT_MSG, 0) == -1){
        perror("init_msg_send: ");
        return -1;
    }
    return 0;
}
int main(){
    srand(time(NULL));
    char filename[] = QUEUEFILENAME;
    key_t fd_queue = ftok(filename, 'A');
    int msgqueue_key = msgget(fd_queue, IPC_CREAT);
    if(msgqueue_key == -1){
        perror("error init queue");
        return -1;
    }
    if(init_msg_send(msgqueue_key)){
        printf("Exit\n");
        return -1;
    }
    printf("Start client_service\n");
    pthread_t thread;
    pthread_create(&thread, NULL, thread_receive_msg, (void*)&msgqueue_key);
    sleep(1);
    struct msg_buf msg;
    msg.mtype = TYPE_MSG_SERVICE;
    msg.init = 0;
    while(1){
        printf("Input: ");
        memset(msg.mtext, 0, SIZE_MSG);
        fgets(msg.mtext, SIZE_MSG, stdin);
        replace_enter(msg.mtext);
        if(strcmp(msg.mtext, "exit") == 0){
            break;
        }
        if(msgsnd(msgqueue_key, &msg, SIZE_STRUCT_MSG, 0) == -1){
            printf("Error: send msg\n");
            return -1;
        }
    }
    exit_msg_send(msgqueue_key);
    printf("End client_service\n");
    return 0;
}