#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>

#define SIZE_MSG 50


int main(){
    char filename[] = "/server.txt";
    #ifdef DElQ
        mq_unlink(filename);
        printf("Delete queue\n");
        return 0;
    #endif
    mqd_t fd_mq;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = SIZE_MSG;
    attr.mq_curmsgs = 0;
    fd_mq = mq_open(filename, O_CREAT | O_RDWR, 0666, &attr);
    if(fd_mq == -1){
        printf("error number is %d \n ",errno);
        perror("init queue");
        return -1;
    }
    char buffer[SIZE_MSG];
    printf("Input message: ");
    fgets(buffer, SIZE_MSG, stdin);
    if(mq_send(fd_mq, buffer, SIZE_MSG, 1) == -1){
        printf("error number is %d \n ",errno);
        perror("send mesg");
        return -1;
    }
    printf("waiting for receipt...\n");
    if(mq_receive(fd_mq, buffer, SIZE_MSG, NULL) == -1){
        printf("error number is %d \n ",errno);
        perror("receive mesg");
        return -1;
    }
    printf("received: %s\n", buffer);
    mq_close(fd_mq);
    printf("End server\n");
    return 0;
}


