#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include "struct_message.h"

int main(){
    char filename[] = ".";
    key_t fd_queue = ftok(filename, 'A');
    int msgqueue_key = msgget(fd_queue, IPC_CREAT | 0660);
    if(msgqueue_key == -1){
        perror("error init queue");
        return -1;
    }
    printf("msgqueue_key = %d\n", msgqueue_key);
    struct msg_buf msg;
    msg.mtype = 1;
    printf("Input message: ");
    fgets(msg.mtext, SIZE_MSG, stdin);
    
    int len;
    len = sizeof(struct msg_buf) - sizeof(long);
    if(msgsnd( msgqueue_key, &msg, len, 0) == -1){
       printf("Error: send msg\n");
       return -1;
    }
    printf("waiting for receipt...\n");
    if(msgrcv( msgqueue_key, &msg, len, 2, 0) == -1){
        printf("Error: receiving msg\n");
        return -1;
    }
    printf("received: %s\n", msg.mtext);
    printf("END\n");

    return 0;
}


