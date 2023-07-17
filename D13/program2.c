#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>



int main(){
    
    printf("\tStart program2\n");
    printf("\tmy pid = %d\n", getpid());
    char buffer[256];
    int len_read = read(0, buffer, sizeof(buffer));
    if(len_read > 0)
        printf("\tread: %d, %s\n", len_read, buffer);
    else
        printf("\treadError: %d\n", len_read);
    printf("\tEnd program2\n");
    return 0;
}


