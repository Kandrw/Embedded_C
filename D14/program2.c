#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>//O_WRONLY
#include <fcntl.h>//mode_t


int main(){
    
    printf("\tStart program2\n");
    printf("\tmy pid = %d\n", getpid());
    char channel_name[] = "channel";
    int fd = open(channel_name, O_WRONLY | O_NONBLOCK);
    if(fd < 0){
        printf("Not found channel: %s\n", channel_name);
        return 0;
    }
    char buffer[] = "sending a message";
    write(fd, buffer, strlen(buffer));
    printf("\tEnd program2\n");
    close(fd);
    return 0;
}


