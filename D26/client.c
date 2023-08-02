#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <sys/poll.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define PORT_SERVER 31415
#define PORT 42566
#define ADDRESS_SERVER "127.0.0.1"
#define ADDRESS "127.0.0.45"

#define SIZE_HEADER 20
#define SIZE_H_UDP 8
#define SIZEBUF 200

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
void *thread_recv_msg(void *args){
    int fd = *(int*)args;
    #ifdef DEBUG
        printf("INIT: %d\n", fd);
    #endif 
    struct udphdr udp_header;
    struct iphdr ip_header;
    char msg_server[SIZEBUF + SIZE_H_UDP + SIZE_HEADER];
    char str[INET_ADDRSTRLEN];
    while(1){
        if(recvfrom(fd, msg_server, sizeof(msg_server), 0, NULL, NULL) == -1){
            perror("recvfrom");
        }
        #ifdef DEBUG
            printf("recv\n");
            for(int i = 0; i < SIZEBUF + SIZE_H_UDP + SIZE_HEADER; ++i){
                printf("%d ", (unsigned char)msg_server[i]);
            }
            printf("\n\n");
        #endif
        for(int i = 0; i < SIZE_HEADER; ++i){
            *((char*)&ip_header+i) = msg_server[i];
        }
        for(int i = 0; i < SIZE_H_UDP; ++i){
            *((char*)&udp_header+i) = msg_server[i+SIZE_HEADER];
        }
        inet_ntop(AF_INET, &ip_header.saddr, str, INET_ADDRSTRLEN);
        #ifdef DEBUG
            printf("source: %s:%d\n", str, ntohs(udp_header.source));
        #endif
        inet_ntop(AF_INET, &ip_header.daddr, str, INET_ADDRSTRLEN);
        #ifdef DEBUG
            printf("dest: %s:%d\n", str, ntohs(udp_header.dest));
            printf("msg: %s\t\n", msg_server + SIZE_H_UDP + SIZE_HEADER);
        #endif

        if( (0 == strcmp(ADDRESS, str)) && (PORT == ntohs(udp_header.dest)) ){
            printf("\tmsg server: %s\t\n", msg_server + SIZE_H_UDP + SIZE_HEADER);
        }
    }

    return NULL;
}

int main(){
    struct sockaddr_in client;
    char buffer[SIZEBUF + SIZE_H_UDP + SIZE_HEADER];
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    memset((char*)&client, 0, sizeof(struct sockaddr_in));
    if(fd < 0){
        perror("socket");
        return -1;
    }
    int setsock = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &setsock, sizeof(setsock))){
        perror("setsockopt");
        return -1;
    }
    pthread_t id;
    pthread_create(&id, NULL, thread_recv_msg, (void**)&fd);
    //pthread_detach(id);
    int size = sizeof(struct sockaddr_in);

    struct udphdr udp_header;
    struct iphdr ip_header;
    memset((char*)&udp_header, 0, SIZE_H_UDP);
    memset((char*)&ip_header, 0, sizeof(struct iphdr));

    udp_header.source = htons(PORT);
    udp_header.dest = htons(PORT_SERVER);
    udp_header.len = htons(SIZEBUF);
        
    ip_header.check = 0;
    ip_header.version = 4;
    ip_header.ihl = 5;
    ip_header.tos = 0;
    ip_header.tot_len = htons(SIZEBUF + SIZE_H_UDP + SIZE_HEADER);
    ip_header.id = 45;
    ip_header.ttl = 64;
    ip_header.protocol = 17;
    udp_header.check = 0;
    inet_pton(AF_INET, ADDRESS, &ip_header.saddr);
    inet_pton(AF_INET, ADDRESS_SERVER, &ip_header.daddr);

    while(1){
        printf("Input: \n");
        fgets(buffer+SIZE_H_UDP + SIZE_HEADER, SIZEBUF-SIZE_H_UDP-SIZE_HEADER, stdin);
        replace_enter(buffer+SIZE_H_UDP+SIZE_HEADER);
        if(!strcmp(buffer+SIZE_H_UDP+SIZE_HEADER, "exit")){
            break;
        }
        for(int i = 0; i < SIZE_HEADER; ++i){
            buffer[i] = *((char*)&ip_header + i);
        }
        for(int i = 0; i < SIZE_H_UDP; ++i){
            buffer[i + SIZE_HEADER] = *((char*)&udp_header + i);
        }
        #ifdef DEBUG
            printf("send\n");
            for(int i = 0; i < SIZEBUF + SIZE_H_UDP + SIZE_HEADER; ++i){
                printf("%d ", (unsigned char)buffer[i]);
            }
            printf("\n");
        #endif
        if(sendto(fd, buffer, SIZEBUF+SIZE_H_UDP+SIZE_HEADER, 0, (struct sockaddr*)&client, size) == -1){
            perror("sendto");
        }
    }
    if(close(fd)){
        perror("close");
    }

    printf("End client\n");
    return 0;
}

