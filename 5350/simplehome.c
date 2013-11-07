#include <stdio.h>

#include "wireless.h"
#include "serial.h"

int main(){
	setupWireless();
	setupSerial();
	while(1){
		sleep(60);
		printf("================oo=============");
	}
	return 0;
}

