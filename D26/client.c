#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <sys/poll.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


/*
    arguments:
    PORT_SERVER
    PORT
    ADDRESS_SERVER
    ADDRESS
*/

#define SIZE_HEADER 20
#define SIZE_H_UDP 8
#define SIZEBUF 200
#define COUNT_ATTEMPT 20

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
void print_headers(char *packet, int len){
	for(int i = 0; i < SIZE_HEADER; ++i){
		printf("%3d  ", (unsigned char)packet[i]);
		if( ((i+1) % 10) == 0){
			printf("\n");
		}
	}
	printf("\n\n");
	for(int i = 0; i < SIZE_H_UDP; ++i){
		printf("%3d  ", (unsigned char)packet[i + SIZE_HEADER]);
	}
	printf("\n\n");
	printf("data: %s\n", packet + SIZE_H_UDP + SIZE_HEADER);
}

void *thread_recv_msg(void *args){
    int fd = *(int*)args;
    #ifdef DEBUG
        printf("INIT: %d\n", fd);
    #endif 
    char msg_server[SIZEBUF + SIZE_H_UDP + SIZE_HEADER];
    char str[INET_ADDRSTRLEN];
    struct iphdr *ip_header = (struct iphdr*)msg_server;
    struct udphdr *udp_header = (struct udphdr*)(msg_server + SIZE_HEADER);
    while(1){
    	memset(msg_server, 0, sizeof(msg_server));
        if(recvfrom(fd, msg_server, sizeof(msg_server), 0, NULL, NULL) == -1){
            perror("recvfrom");
        }
        #ifdef DEBUG
            printf("recv:\n");
            print_headers(msg_server, sizeof(msg_server));
        #endif
        inet_ntop(AF_INET, &ip_header->saddr, str, INET_ADDRSTRLEN);
        if(ntohs(udp_header->source) != PORT && (strcmp(str, ADDRESS) != 0)){
            printf("[%s:%d] msg: %s\n", str, ntohs(udp_header->source), msg_server + SIZE_H_UDP + SIZE_HEADER);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]){

    char buffer[SIZEBUF + SIZE_H_UDP + SIZE_HEADER];
    memset(buffer, 0, sizeof(buffer));
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    struct sockaddr_in client;
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
    client.sin_family = AF_INET;
    inet_pton(AF_INET, ADDRESS_SERVER, &client.sin_addr);
    client.sin_port = htons(PORT_SERVER);
    pthread_t id;
    pthread_create(&id, NULL, thread_recv_msg, (void**)&fd);
    int size = sizeof(client);
    struct udphdr *udp_header = (struct udphdr*)(buffer + SIZE_HEADER);
    struct iphdr *ip_header = (struct iphdr *)buffer;
    udp_header->source = htons(PORT);
    udp_header->dest = htons(PORT_SERVER);
    udp_header->len = htons(SIZEBUF + SIZE_H_UDP);
    udp_header->check = 0;
    ip_header->version = 4;
    ip_header->ihl = 5;
    ip_header->tos = 0;
    ip_header->tot_len = htons(SIZEBUF + SIZE_H_UDP + SIZE_HEADER);
    ip_header->id = 45;
    ip_header->frag_off = 0;
    ip_header->ttl = 64;
    ip_header->protocol = 17;
    ip_header->check = 0;
    buffer[6] = 64;
    inet_pton(AF_INET, ADDRESS, &ip_header->saddr);
    inet_pton(AF_INET, ADDRESS_SERVER, &ip_header->daddr);

    while(1){
    	memset(buffer + SIZE_HEADER + SIZE_H_UDP, 0, SIZEBUF);
        printf("Input: \n");
        fgets(buffer+SIZE_H_UDP + SIZE_HEADER, SIZEBUF-SIZE_H_UDP-SIZE_HEADER, stdin);
        replace_enter(buffer+SIZE_H_UDP+SIZE_HEADER);
        if(!strcmp(buffer+SIZE_H_UDP+SIZE_HEADER, "exit")){
            break;
        }
        #ifdef DEBUG
            printf("send:\n");
            print_headers(buffer, sizeof(buffer));
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
