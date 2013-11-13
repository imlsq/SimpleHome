#include <stdio.h>
#include <string.h>

#include "serial.h"


const char *executeOpenCmd(char *cmd){
	char openCmd[strlen(cmd)+5];
	strncpy(openCmd,cmd,strlen(cmd)+3);	
	const char *rece=syn_write_serial(openCmd,1000);
	if(strstr(rece,"ON")!=NULL){
		return "ON\r\n";
	}
	return "FAIL\r\n";
}
