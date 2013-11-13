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
#include "serial.h"
#include "openAction.h"


#define MAX_BUFF_SIZE 256
#define MAX_SEND_PACKET_SIZE 512
#define MAX_CLIENT_CONNECTIONS 3
#define MAX_PACKET_SIZE 128



struct client_sock_record cli_sock_rcd_lst[MAX_CLIENT_CONNECTIONS];
int lst_next = 0;

void Die(char* mess)
{
        perror(mess);
        exit(1);
}



void write_socket_port(const char *buf){		
		int i=0;
		while(i<lst_next){
			send(cli_sock_rcd_lst[i].sock, buf, sizeof buf, 0);
			i++;
		}
}

int readline(int sockfd, char *response){
    char c = '0';
    int status = 0, i = 0;

	memset(response, 0, sizeof response);
    while(1){
        status = recv(sockfd,&c, 1,0);
        if(status <=0){
            return -1;
        }        
        response[i] = c;		
		i++;

		if(c == '\n' || c=='#'){			
			response[i] = '\0';			
			break;
		}
    }
    return i;
}

void removeSocket(int sockefd){
	int i;
	i=0;
	while(i<MAX_CLIENT_CONNECTIONS){
		if(cli_sock_rcd_lst[i].sock == sockefd){
			cli_sock_rcd_lst[i].sock=0;
			lst_next--;
			if(lst_next<0){
				lst_next=0;
			}
			break;
		}
		i++;
	}
	
	
	i=0;
	for(i=0;i<MAX_CLIENT_CONNECTIONS-1;i++){
		if(cli_sock_rcd_lst[i].sock==0){
			cli_sock_rcd_lst[i]=cli_sock_rcd_lst[i+1];
		}
	}
}

void* branch_from_input (void *sockfd_p)
{
        char response[1024];
		
        int sockfd = *((int*)sockfd_p);
		int ret;
		
        while(1){
				ret = readline(sockfd,response);
				if(ret<0){
					printf("Exit current thread:%d\n",sockfd);
					removeSocket(sockfd);
					pthread_exit(NULL);
				}				
				const char *actionResponse;
				if(strstr(response, "OPEN") !=NULL){					
					actionResponse = executeOpenCmd(response);
				}else if(strstr(response, "CLOSE") !=NULL){
					printf("Close action\n");
				}else if(strstr(response, "GET_INFO") !=NULL){
					printf("Get info action\n");
				}else if(strstr(response, "SCHEDULES") !=NULL){
					printf("Schedules action\n");
				}
				printf("Action result:%s",actionResponse);
				write_socket_port(actionResponse);                
        }
}

void start_socket_server()
{
        int serv_sockfd;
		int sock;
        struct sockaddr_in serv_addr;

        if ((serv_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                Die("ERROR opening socket");

        memset(&serv_addr, 0, sizeof serv_addr);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(3333);

		int opt=SO_REUSEADDR;
		setsockopt(serv_sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
        if (bind(serv_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                Die("ERROR on binding");
        listen(serv_sockfd,5);

        while (1){
                cli_sock_rcd_lst[lst_next].client_len = sizeof cli_sock_rcd_lst[lst_next].cli_addr;
                sock = accept(
                                serv_sockfd,
                                (struct sockaddr *) &cli_sock_rcd_lst[lst_next].cli_addr,
                                (socklen_t*) &cli_sock_rcd_lst[lst_next].client_len
                );

				if(sock>0){					
					pthread_t ntid;
					int err = pthread_create(&ntid, NULL, &branch_from_input, &sock);
					cli_sock_rcd_lst[lst_next].sock=sock;
					lst_next++;
				}
        }
        
}
