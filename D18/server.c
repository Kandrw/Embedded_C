#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <pthread.h>

#include "struct_message.h"

struct List{
    char str[SIZE_MSG];
    struct List *next;
};
pthread_mutex_t mutex_exit;
int con_exit = 0;

struct List *Create_List(){
    struct List *new_element = (struct List*)malloc(sizeof(struct List));
    new_element->next = NULL;
    return new_element;
}
struct List *Delete_List_elem(struct List *head, char *str){
    struct List *ptr_del;
    while(head->next){
        if(strcmp(head->next->str, str) == 0){
            ptr_del = head->next;
            head->next = head->next->next;
            free(ptr_del);
            if( !head->next){
                return head;
            }
            else{
                return NULL;
            }
        }
        head = head->next;
    }
    return NULL;
}
void Print_List(struct List *head){
    head = head->next;
    while(head){
        printf("%s\n", head->str);
        head = head->next;
    }
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
void *thread_input_com(void *args){
    char buffer[20];
    printf("Start thread\n");
    while(1){
        fgets(buffer, sizeof(buffer), stdin);
        replace_enter(buffer);
        if(strcmp(buffer, "exit") == 0){
            pthread_mutex_lock(&mutex_exit);
            con_exit = 1;
            pthread_mutex_unlock(&mutex_exit);
            break;
        }
    }
    return NULL;
}

int main(){
    char filename[] = QUEUEFILENAME;
    key_t fd_queue = ftok(filename, 'A');
    int msgqueue_key = msgget(fd_queue, IPC_CREAT | 0660);
    if(msgqueue_key == -1){
        perror("error init queue");
        return -1;
    }
    printf("Start server\n");
    printf("key %d\n", msgqueue_key);
    
    struct msg_buf msg;
    msg.mtype = 1;
    struct List *head = Create_List();
    struct List *end = head, *ptr;
    int count_user = 0;
    pthread_t thread;
    pthread_create(&thread, NULL, thread_input_com, (void**)NULL);

    while(1){
        memset(msg.mtext, 0, SIZE_MSG);
        if(msgrcv( msgqueue_key, &msg, SIZE_STRUCT_MSG, 0, 0) == -1){
            printf("Error: receiving msg\n");
            return -1;
        }
        if(msg.init == 1){
            count_user++;
            ptr = Create_List();
            strcpy(ptr->str, msg.mtext);
            end->next = ptr;
            end = ptr;
            printf("Count user %d \n", count_user);
            Print_List(head);
        }
        else if(msg.init == 2){
            ptr = Delete_List_elem(head, msg.mtext);
            if(ptr){
                end = ptr;
            }
            count_user--;
            printf("Count user %d \n", count_user);
            Print_List(head);
        }
        else if(msg.mtype == TYPE_MSG_SERVICE){
            msg.init = 0;
            msg.mtype = TYPE_SENDING_SERVICE_MSG;
            ptr = head->next;
            while(ptr){
                strcpy(msg.mtext, ptr->str);
                if(msgsnd(msgqueue_key, &msg, SIZE_STRUCT_MSG, 0) == -1){
                    printf("Error: send msg\n");
                    return -1;
                }
                printf("\t[%s]\n", msg.mtext);
                ptr = ptr->next;
            }
        }
        else{
            msg.init = 0;
            msg.mtype = TYPE_SENDING_MSG;
            for(int i = 0; i < count_user; ++i){
                if(msgsnd(msgqueue_key, &msg, SIZE_STRUCT_MSG, 0) == -1){
                    printf("Error: send msg\n");
                    return -1;
                }
            }
        }
        printf("-> msg = %s, p = %d, init = %d\n", msg.mtext, msg.mtype, msg.init);
        pthread_mutex_lock(&mutex_exit);
        if(con_exit == 1){
            break;
        }
        pthread_mutex_unlock(&mutex_exit);
    }
    while(head){
        ptr = head;
        head = head->next;
        free(ptr);
    }
    int *s;
    pthread_join(thread, (void**)&s);
    pthread_mutex_destroy(&mutex_exit);
    printf("End server\n");
    return 0;
}







