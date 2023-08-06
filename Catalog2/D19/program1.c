#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>// ftruncate

#include <semaphore.h>

#define SIZE 100

//#define DELMEM

int main(){
    char filename_memory[] = "/shm";
    char filename_semapfore[] = "/sem";
    #ifdef DELMEM
        sem_unlink(filename_semapfore);
    return 0;
    #endif
    // init semapfore
    sem_t *sem = sem_open(filename_semapfore, O_CREAT, 0777, 1);
    if(sem == SEM_FAILED){
        perror("sem_open");
        return -1;
    }
    // init memory
    int fd = shm_open(filename_memory, O_CREAT | O_RDWR, 0777);
    if(fd == -1){
        perror("shm_open");
        return -1;
    }
    if(ftruncate(fd, SIZE) == -1){
        perror("ftruncate");
        return -1;
    }
    char *ptr = mmap(NULL, SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if(ptr == (char*)-1){
        printf("%d\n", errno);
        perror("mmap");
        return -1;
    }
    printf("Input msg: ");
    sem_wait(sem);
    fgets(ptr, SIZE, stdin);
    sem_post(sem);
    sleep(1);
    sem_wait(sem);
    printf("received msg: %s\n", ptr);
    sem_post(sem);
    sleep(1);
    munmap(ptr, SIZE);
    if(shm_unlink(filename_memory) == -1){
        perror("shm_unlink");
        return -1;
    }
    sem_close(sem);
    sem_unlink(filename_semapfore);
    printf("End\n");
    return 0;
}



