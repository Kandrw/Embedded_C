#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define COUNT_SHOP 5
#define COUNT_PRODUCTS 1000
#define COUNT_CONSUMER 3
#define COUNT_NEEDS 3000
#define COUNT_SESSION 300
#define COUNT_LOAD_SHOP 100


int shop[COUNT_SHOP];
pthread_mutex_t mutex_consumers[COUNT_SHOP];
int needs_consumer[COUNT_CONSUMER];
int consumers_in_need = COUNT_CONSUMER;
pthread_mutex_t mutex_remaining_consumers = PTHREAD_MUTEX_INITIALIZER;

void *loader_shop(void *args){
    int i;
    int run = 1;
    while(run){
        for(i = 0; i < COUNT_SHOP; ++i){
            pthread_mutex_lock(mutex_consumers + i);
            shop[i] += COUNT_LOAD_SHOP;
            printf("load in shop, shop[%d] = %d\n", i, shop[i]);
            fflush(stdout);
            pthread_mutex_unlock(mutex_consumers + i);
            pthread_mutex_lock(&mutex_remaining_consumers);
            if(consumers_in_need <= 0){
                pthread_mutex_unlock(&mutex_remaining_consumers);
                run = 0;
                break;
            }
            pthread_mutex_unlock(&mutex_remaining_consumers);
            sleep(1);
        }
        
    }
    return NULL;
}
void *consumer_shop(void *args){
    int *ptr = (int*)args;
    int i;
    while(needs_consumer[*ptr] > 0){
        for(i = 0; i < COUNT_SHOP; ++i){
            pthread_mutex_lock(mutex_consumers + i);
            fflush(stdout);
            if(shop[i] == 0){
            }
            else if(needs_consumer[*ptr] >= shop[i]){
                if(shop[i] >= COUNT_SESSION){
                    needs_consumer[*ptr] -= COUNT_SESSION;
                    shop[i] -= COUNT_SESSION;
                }
                else{
                    needs_consumer[*ptr] -= shop[i];
                    shop[i] = 0;
                }
            }
            else{
                if(needs_consumer[*ptr] >= COUNT_SESSION){
                    shop[i] -= COUNT_SESSION;
                    needs_consumer[*ptr] -= COUNT_SESSION;
                }
                else{
                    shop[i] -= needs_consumer[*ptr];
                    needs_consumer[*ptr] = 0;
                }
            }
            printf("cons - %d, needs - %d, shop[%d] = %d\n", *ptr, needs_consumer[*ptr], i, shop[i]);
            fflush(stdout);
            pthread_mutex_unlock(mutex_consumers + i);
        }
    }
    pthread_mutex_lock(&mutex_remaining_consumers);
    --consumers_in_need;
    printf("count consumers_in_need = %d\n", consumers_in_need);
    fflush(stdout);
    pthread_mutex_unlock(&mutex_remaining_consumers);
    printf("End thread %d\n", *ptr);
    return NULL;
}

int main(){
    int i;
    for(i = 0; i < COUNT_SHOP; ++i){
        shop[i] = COUNT_PRODUCTS;
        pthread_mutex_init(mutex_consumers + i, NULL);
        printf("%d\t", shop[i]);
    }
    printf("\n");
    for(i = 0; i < COUNT_CONSUMER; ++i){
        needs_consumer[i] = COUNT_NEEDS;
        printf("%d\t", needs_consumer[i]);
    }
    printf("\n");
    pthread_t consumers[COUNT_CONSUMER];
    pthread_t loader_thread;
    int *s;
    int number_thread[COUNT_CONSUMER];
    for(i = 0; i < COUNT_CONSUMER; ++i){
        number_thread[i] = i;
        pthread_create(consumers + i, NULL, consumer_shop, (void*)(number_thread+i));
    }
    pthread_create(&loader_thread, NULL, loader_shop, (void**)NULL);
    for(i = 0; i < COUNT_CONSUMER; ++i){
        pthread_join(consumers[i], (void**)&s);
    }
    pthread_join(loader_thread, (void**)&s);
    for(i = 0; i < COUNT_SHOP; ++i){
        pthread_mutex_destroy(mutex_consumers + i);
    }
    pthread_mutex_destroy(&mutex_remaining_consumers);
    printf("End program\n");
    return 0;
}

