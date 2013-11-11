#include <stdio.h>

#include "serial.h"

char *executeOpenCmd(char cmd){
	printf("Execute open action\n");
	syn_write_serial("open\r\n",1000);
	
	return "ON";
}
