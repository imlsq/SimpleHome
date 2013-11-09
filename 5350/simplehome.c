#include <stdio.h>

#include "wireless.h"
#include "serial.h"
#include "socketserver.h"

int main(){
	//setupWireless();
	//startSerial();
	start_socket_server();
	while(1){
		sleep(60);
		printf("================oo=============");
	}
	return 0;
}

