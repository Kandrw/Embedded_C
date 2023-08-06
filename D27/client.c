#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <sys/poll.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <net/if.h>

#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> 

#define SIZE_H_ET 14
#define SIZE_H_IP 20
#define SIZE_H_UDP 8
#define SIZE_DATA 200
#define SIZE_PACKET (SIZE_DATA + SIZE_H_UDP + SIZE_H_IP + SIZE_H_ET)

enum pos_argv{
    DEVICE = 1,
    SMAC,
    DMAC,
    SADDR,
    DADDR,
    SPORT,
    DPORT
};

int port;
int port_server;
char *address;
char *address_server;

void replace_enter(char *str);
void convert_mac(const char *str_mac, char *mac);
void print_headers(char *packet, int len);
short check_sum_ip(void *args, int N);
void check_sum_udp(struct iphdr *pIph, unsigned short *data);
void *thread_recv_msg(void *args);

int main(int argc, char *argv[]){
    if(argc < 7){
        printf("Not enough arguments\n");
        printf("\targuments: device name, source mac, dest mac, saddr, daddr, source, dest\n");
        printf("Exit\n");
        return -1;
    }
    unsigned int index_device = if_nametoindex(argv[DEVICE]);
    #ifdef DEBUG
        printf("index = %d\n", index_device);
    #endif
    if(index <= 0){
        printf("Not found device: %s\n", argv[DEVICE]);
        return -1;
    }
    if(sscanf(argv[SPORT], "%d", &port) != 1){
        printf("Error: incorrect source port %s\n", argv[SPORT]);
        return -1;
    }
    if(sscanf(argv[DPORT], "%d", &port_server) != 1){
        printf("Error: incorrect destination port %s\n", argv[DPORT]);
        return -1;
    }
    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(fd < 0){
        perror("socket");
        return -1;
    }
    char buffer[SIZE_PACKET];
    memset(buffer, 0, sizeof(buffer));
    address = argv[SADDR];
    address_server = argv[DADDR];
    pthread_t id;
    pthread_create(&id, NULL, thread_recv_msg, (void**)&fd);

    struct sockaddr_ll addrll;
    memset((void*)&addrll, 0, sizeof(addrll));
    addrll.sll_family = AF_PACKET;
    addrll.sll_protocol = htons(ETH_P_ALL);
    addrll.sll_ifindex = index_device;  
    addrll.sll_halen = ETH_ALEN;
    convert_mac(argv[SMAC], (char*)addrll.sll_addr);

    struct ethhdr *eth_header = (struct ethhdr*)buffer;
    struct iphdr *ip_header = (struct iphdr *)(buffer + SIZE_H_ET);
    struct udphdr *udp_header = (struct udphdr*)(buffer + SIZE_H_IP + SIZE_H_ET);
    
    convert_mac(argv[DMAC], (char*)eth_header->h_dest);
    convert_mac(argv[SMAC], (char*)eth_header->h_source);
    eth_header->h_proto = htons(ETH_P_IP);

    ip_header->version = 4;
    ip_header->ihl = 5;
    ip_header->tos = 0;
    ip_header->tot_len = htons(SIZE_DATA + SIZE_H_UDP + SIZE_H_IP);
    ip_header->id = 45;
    ip_header->frag_off = 0;
    ip_header->ttl = 64;
    ip_header->protocol = 17;
    ip_header->check = 0;
    buffer[6 + SIZE_H_ET] = 64;

    udp_header->source = htons(port);
    udp_header->dest = htons(port_server);
    udp_header->len = htons(SIZE_DATA + SIZE_H_UDP);
    udp_header->check = 0;

    if(inet_pton(AF_INET, argv[SADDR], &ip_header->saddr) <= 0){
        printf("Error: incorrect source address %s\n", argv[SADDR]);
        close(fd);
        return -1;
    }
    if(inet_pton(AF_INET, argv[DADDR], &ip_header->daddr) <= 0){
        printf("Error: incorrect destination address %s\n", argv[DADDR]);
        close(fd);
        return -1;
    }
    
    while(1){
    	memset(buffer + SIZE_PACKET - SIZE_DATA, 0, SIZE_DATA);
        ip_header->check = 0;
        printf("Input: \n");
        fgets(buffer + SIZE_PACKET - SIZE_DATA, SIZE_DATA, stdin);
        replace_enter(buffer + SIZE_PACKET - SIZE_DATA);
        if(!strcmp(buffer + SIZE_PACKET - SIZE_DATA, "exit")){
            break;
        }
        ip_header->check = ~check_sum_ip((void*)ip_header, SIZE_H_IP/2);
        check_sum_udp(ip_header, (unsigned short*)(buffer + SIZE_H_IP + SIZE_H_ET));
        #ifdef DEBUG
            printf("[%s:%d] send:\n", argv[SADDR], port);
            print_headers(buffer, sizeof(buffer));
            printf("ip chek_sum = %hd, ~%hd\n", ~ip_header->check, ip_header->check);
            printf("udp chek_sum = %hd, ~%hd\n", ~udp_header->check, udp_header->check);
        #endif
        if(sendto(fd, buffer, SIZE_PACKET, 0, (struct sockaddr*)&addrll, sizeof(addrll)) == -1){
            perror("sendto");
        }
        printf("sendto [%s|%s|%d]\n", argv[DMAC], argv[DADDR], port_server);
    }
    if(close(fd)){
        perror("close");
    }
    printf("End client\n");
    return 0;
}

void *thread_recv_msg(void *args){
    int fd = *(int*)args;
    #ifdef DEBUG
        printf("INIT: %d\n", fd);
    #endif 
    char msg_server[SIZE_PACKET];
    memset(msg_server, 0, sizeof(msg_server));
    struct iphdr *ip_header = (struct iphdr *)(msg_server + SIZE_H_ET);
    struct udphdr *udp_header = (struct udphdr*)(msg_server + SIZE_H_IP + SIZE_H_ET);
    int chsm_ip, chsm_udp;
    char str_daddr[INET_ADDRSTRLEN], str_saddr[INET_ADDRSTRLEN];
    while(1){
    	memset(msg_server, 0, sizeof(msg_server));
        if(recvfrom(fd, msg_server, sizeof(msg_server), 0, NULL, NULL) == -1){
            perror("recvfrom");
        }
        inet_ntop(AF_INET, &ip_header->saddr, str_saddr, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &ip_header->daddr, str_daddr, INET_ADDRSTRLEN);
        #ifdef DEBUG
            printf("[%s:%d] recv:\n", str_saddr, ntohs(udp_header->source));
            print_headers(msg_server, sizeof(msg_server));
            printf("old: ip chek_sum = %hd,  ", ip_header->check);
            printf("udp chek_sum = %hd\n", udp_header->check);
        #endif
        chsm_ip = ip_header->check;
        ip_header->check = 0;
        ip_header->check = ~check_sum_ip((void*)ip_header, SIZE_H_IP/2);
        chsm_udp = udp_header->check;
        check_sum_udp(ip_header, (unsigned short*)(msg_server + SIZE_H_IP + SIZE_H_ET));
        #ifdef DEBUG
            printf("new: ip chek_sum = %hd,  ", ip_header->check);
            printf("udp chek_sum = %hd\n", udp_header->check);
            if(chsm_ip != ip_header->check){
                printf("incorrect check sum ip header\n");
            }
            if(chsm_udp != udp_header->check){
                printf("incorrect check sum udp header\n");
            }
        #endif
        if( (chsm_ip == ip_header->check) && (chsm_udp == udp_header->check) &&\
            (ntohs(udp_header->source) == port_server) && (strcmp(str_saddr, address_server) == 0) &&\
            (ntohs(udp_header->dest) == port) && (strcmp(str_daddr, address) == 0) )
        {
            printf("[%s:%d] msg server: %s\n", str_saddr, ntohs(udp_header->source), msg_server + SIZE_H_UDP + SIZE_H_IP + SIZE_H_ET);
        }
    }
    return NULL;
}

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
void convert_mac(const char *str_mac, char *mac){
    for(int i = 0; i < 18; i += 3){
        sscanf(str_mac+i, "%2hhx", mac+(i/3));
    }
}
void print_headers(char *packet, int len){
    for(int i = 0; i < SIZE_H_ET; ++i){
		printf("%3d  ", (unsigned char)packet[i]);
	}
    printf("\n\n");
	for(int i = SIZE_H_ET; i < SIZE_H_IP + SIZE_H_ET; ++i){
		printf("%3d  ", (unsigned char)packet[i]);
	}
	printf("\n\n");
	for(int i = SIZE_H_ET + SIZE_H_IP; i < SIZE_H_UDP + SIZE_H_IP + SIZE_H_ET; ++i){
		printf("%3d  ", (unsigned char)packet[i]);
	}
	printf("\n\n");
	printf("data: %s\n", packet + SIZE_H_UDP + SIZE_H_IP + SIZE_H_ET);
}
short check_sum_ip(void *args, int N){
    int i_cksm = 0;
    unsigned short *ptr = (unsigned short*)args;
    for(int i = 0; i < N; ++i){
        i_cksm += *ptr;
        ptr++;
    }
    unsigned short cksm = (unsigned short)(i_cksm & 0xFFFF) + (unsigned short)(i_cksm>>16);
    cksm = (unsigned short)(i_cksm & 0xFFFF) + (unsigned short)(i_cksm>>16);
    return cksm;
}
void check_sum_udp(struct iphdr *pIph, unsigned short *data) {
    register unsigned long sum = 0;
    struct udphdr *udphdrp = (struct udphdr*)(data);
    unsigned short udpLen = htons(udphdrp->len);
    sum += ((pIph->saddr>>16)&0xFFFF) + ((pIph->saddr)&0xFFFF);
    sum += ((pIph->daddr>>16)&0xFFFF) + ((pIph->daddr)&0xFFFF);
    sum += htons(IPPROTO_UDP);
    sum += udphdrp->len;
    udphdrp->check = 0;
    while (udpLen > 1) {
        sum += * data++;
        udpLen -= 2;
    }
    if(udpLen > 0) {
        sum += ((*data)&htons(0xFF00));
    }
    while (sum>>16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    udphdrp->check = (unsigned short)~sum;
}

