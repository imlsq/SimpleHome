
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <termios.h>

#include	"nvram.h"

pthread_t wireless_tid; 

void *configWireless(){
	sleep(8);

	char *ssid , *passwd;
	char *psk="iwpriv ra0 set WPAPSK=";
	char *iwssidcmd="iwpriv ra0 set SSID=";

	printf("setup wireless..... \n");

	ssid = nvram_get(RT2860_NVRAM,"simple_home_ssid");
	printf("ssid:%s\n",ssid);
	passwd = nvram_get(RT2860_NVRAM,"simple_home_passwd");
	printf("passwd:%s\n",passwd);
	system("iwpriv ra0 set NetworkType=Infra");
	system("iwpriv ra0 set AuthMode=WPA2PSK");
	system("iwpriv ra0 set EncrypType=AES");

	char *buffer = malloc (strlen (passwd) + strlen (psk)  + 1);
	if (buffer == NULL) {
		
	} else {
		strcpy (buffer, psk);
		strcat (buffer, passwd);
		printf("%s\n",buffer);
		system(buffer);
	}
	free (buffer);


	*buffer = malloc (strlen (ssid) + strlen (iwssidcmd)  + 1);
	if (buffer == NULL) {
		
	} else {
		strcpy (buffer, iwssidcmd);
		strcat (buffer, ssid);
		printf("%s\n",buffer);
		system(buffer);
	}
	free (buffer);
	pthread_exit(NULL);
}

void setupWireless(){
	int ret;
	ret = pthread_create(&wireless_tid, NULL, configWireless, NULL);
    if (ret < 0)
        printf("Create wireless config thread error.");

}


