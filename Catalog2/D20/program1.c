#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SIZE 100

int main(){

    char filename_mem[] = ".";
    char filename_sem[] = "program1";
    
    key_t fd_key = ftok(filename_mem, 'A');
    if(fd_key < 0){
        perror("ftok mem");
        return -1;
    }
    printf("fd_key = 0x%x\n", fd_key);
    int fd = shmget(fd_key, SIZE, IPC_CREAT | 0666);
    if(fd < 0){
        printf("errno = %d\n", errno);
        perror("shmget");
        return -1;
    }
    char * ptr = (char*)shmat(fd, NULL, 0);
    if(ptr == (char*)-1){
        printf("errno = %d\n", errno);
        perror("shmat");
        return -1;
    }
    printf("Init memory\n");

    
    key_t fd_key_sem = ftok(filename_sem, 'B');
    if(fd_key_sem < 0){
        perror("ftok sem");
        return -1;
    }
    printf("fd_key_sem = 0x%x\n", fd_key_sem);
    int fd_sem = semget(fd_key_sem, 1, IPC_CREAT | 0666);
    if(fd_sem < 0){
        printf("errno = %d\n", errno);
        perror("semget sem");
        return -1;
    }
    struct sembuf sem1;
    sem1.sem_num = 0;
    sem1.sem_flg = 0;
    sem1.sem_op = 167;
    int res = semctl (fd_sem, 0, GETVAL, 0); 
    printf("sem = %d\n", res);
    
    printf("Input msg: ");
    fgets(ptr, SIZE, stdin);

    semop(fd_sem, &sem1, 1);
    res = semctl (fd_sem, 0, GETVAL, 0); 
    printf("sem = %d\n", res);
    sem1.sem_op *= -1;
    printf("waiting for a message from program2\n");
    semop(fd_sem, &sem1, 1);
    printf("received: %s\n", ptr);

    sleep(3);
    // Delete resource

    if(shmdt(ptr) < 0){
        printf("errno = %d\n", errno);
        perror("shmdt");
        return -1;
    }
    if(shmctl(fd, IPC_RMID, 0) < 0){
        printf("errno = %d\n", errno);
        perror("shmctl");
        return -1;
    }
    printf("Delete memory\n");

    if(semctl(fd_sem, 0, IPC_RMID, 0) < 0){
        printf("errno = %d\n", errno);
        perror("semctl sem");
        return -1;
    }
    printf("Delete semaphore\n");

    printf("End program1\n");
    return 0;
}

