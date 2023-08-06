#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <semaphore.h>

#define SIZE 100

int main(){
    char filename_memory[] = "/shm";
    char filename_semapfore[] = "/sem";
    // init semapfore
    sem_t *sem = sem_open(filename_semapfore, 0);
    if(sem == SEM_FAILED){
        perror("sem_open");
        return -1;
    }
    // init memory
    int fd = shm_open(filename_memory, O_RDWR, 0777);
    if(fd == -1){
        perror("shm_open\n");
        return -1;
    }
    if(ftruncate(fd, SIZE) == -1){
        perror("ftruncate\n");
        return -1;
    }
    char *ptr = mmap(NULL, SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if(ptr == (char*)-1){
        perror("mmap\n\n");
        return -1;
    }
    char buf[SIZE];
    printf("waiting for a message...\n");
    sem_wait(sem);
    strcpy(buf, ptr);
    printf("received: %s\n", buf);
    printf("Input msg: ");
    fgets(ptr, SIZE, stdin);
    sem_post(sem);
    printf("End\n");
    return 0;
}



