
#include <netinet/in.h>
#ifndef SIMPLE_SERVER_H_
#define SIMPLE_SERVER_H_
#endif /* SIMPLE_SERVER_H_ */

void start_socket_server();
struct client_sock_record
{
        int sock;
        struct sockaddr_in cli_addr;
        int client_len;
};

