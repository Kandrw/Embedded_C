#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void func_sigusr1(int sig, siginfo_t *siginf, void *ptr){
    printf("pid process: %d\n", siginf->si_pid);
    kill(siginf->si_pid, SIGUSR2);
    printf("Exit program1\n");
    exit(0);
}
 
int main(void){
    printf("mypid = %d\n", getpid());
    struct sigaction act = { 0 };

    act.sa_sigaction = func_sigusr1;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    while(1){
    }
    printf("End program1\n");
    return 0;
}



