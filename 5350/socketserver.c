#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "socketserver.h"


#define MAX_BUFF_SIZE 256
#define MAX_SEND_PACKET_SIZE 512
#define MAX_CLIENT_CONNECTIONS 32
#define MAX_PACKET_SIZE 128



struct client_sock_record cli_sock_rcd_lst[MAX_CLIENT_CONNECTIONS];
int lst_next = 0;

void Die(char* mess)
{
        perror(mess);
        exit(1);
}

int send_server_time(int sockfd)
{
        char packet[MAX_SEND_PACKET_SIZE];
        size_t packet_len;

        time_t t = time(NULL);
        struct tm time = *localtime(&t);

        packet_len = (size_t)sprintf(packet,"Server Time: %d-%d-%d %d:%d:%d\n",
                        time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

        if(send(sockfd, (const void *) packet, packet_len, 0) == packet_len)
                return 1;
        else{
                Die("Failed to send the time packet to the client");
                return -1;
        }
}

int send_server_name(int sockfd)
{
        char packet[MAX_SEND_PACKET_SIZE];
        size_t packet_len;

        packet_len = (size_t)sprintf(packet,"Welcome to Samuel's word\n");

        if(send(sockfd, (const void *) packet, packet_len, 0) == packet_len)
                return 1;
        else
                Die("Failed to send the time packet to the client");

        return -1;
}

void* branch_from_input (void *sockfd_p)
{
        char pkt_header[3];
        int sockfd = *((int*)sockfd_p);

        memset(pkt_header, 0, sizeof pkt_header);

        while(1){
                if (recv(sockfd, pkt_header, sizeof pkt_header, 0) < 0 || pkt_header[2] != '\n'){
                        printf("packet header format error:%s\n",pkt_header);
						
                }else{
                        switch(pkt_header[0]){
                        case 'T':
                                send_server_time(sockfd);
                                break;
                        case 'N':
                                send_server_name(sockfd);
                                break;
                        default:
                                printf("packet header format error\n");
                                break;
                        }
                }
        }
}

void start_socket_server()
{
        int serv_sockfd;
        struct sockaddr_in serv_addr;

        /* First call to socket() function */
        if ((serv_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                Die("ERROR opening socket");

        /* Initialize socket structure */
        memset(&serv_addr, 0, sizeof serv_addr);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(3333);

        if (bind(serv_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                Die("ERROR on binding");
        listen(serv_sockfd,5);

        while (1){
                //FIXME problems
                cli_sock_rcd_lst[lst_next].client_len = sizeof cli_sock_rcd_lst[lst_next].cli_addr;
                cli_sock_rcd_lst[lst_next].sock = accept(
                                serv_sockfd,
                                (struct sockaddr *) &cli_sock_rcd_lst[lst_next].cli_addr,
                                (socklen_t*) &cli_sock_rcd_lst[lst_next].client_len
                );

                if ( cli_sock_rcd_lst[lst_next].sock < 0)
                        Die("ERROR on accept");
                
                pthread_t ntid;
                int err = pthread_create(&ntid, NULL, &branch_from_input, &cli_sock_rcd_lst[lst_next].sock);
                lst_next++;
                if(err < 0)
                        Die("thread create error");
        }
        
}
