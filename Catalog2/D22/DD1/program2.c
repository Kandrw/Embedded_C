#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int run = 1;

void func_sigusr2(int sig, siginfo_t *siginf, void *ptr){
    printf("pid process: %d\n", siginf->si_pid);
    run = 0;
}
int main(void){
    printf("mypid = %d\n", getpid());
    struct sigaction act = { 0 };
    act.sa_sigaction = func_sigusr2;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR2, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    int pid;
    printf("Input pid: ");
    if(scanf("%d", &pid) > 0){
        kill((pid_t)pid, SIGUSR1);
    }
    while(run){
    }
    printf("End program2\n");
    return 0;
}



