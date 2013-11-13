#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <termios.h>



int serialPort;

char *receiver;
pthread_t read_tid; 


void *read_port_thread(){
    int num,i=0;
    char tmp;
	char serialResponse[1024*1024];
    while(1)
    {
        while ((num=read(serialPort, &tmp, 1))>0)
        {            
			if(i>=sizeof serialResponse){
				i=0;
			}
			serialResponse[i]=tmp;			
			i++;
            if(tmp=='\n'){
				serialResponse[i]='\0';
				i=0;
				char tmp_receiver[strlen(serialResponse)];
				strcpy(tmp_receiver,serialResponse);
				receiver=tmp_receiver;				
			}		
        }
		
        if (num < 0) 
            pthread_exit(NULL);
     }
	 pthread_exit(NULL);
     
}



void write_serial_port(char *buf){

    int ret;
	/*
    static int cnt = 1;
    char send_buf[512] = {0};
    sprintf(send_buf, "%d %s", cnt, buf);
	*/
	ret = write(serialPort, buf, strlen(buf));
}

const char *syn_write_serial(char *buf, int *timeout){
	receiver="";
	write_serial_port(buf);
	int step=2;
	while(timeout>0){
		if(receiver !=""){
			return receiver; 
		}
		usleep(step);
		timeout=timeout-step;
	}
	return "";
}

int open_port(const char* device)
{
    int fd = -1; /* File descriptor for the port, we return it. */
    int ret;

    fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK); // O_RDWR | O_NOCTTY | O_NDELAY
    if (fd == -1)
        printf("Unable to open the port\n");

    /* block */
    ret = fcntl(fd, F_SETFL, 0);
    printf("fcntl %s\n",ret);

    ret = isatty(STDIN_FILENO);
    if (ret == 0)
        printf("Standard input is not a terminal device.\n");
    printf("Open the port %s success!\n",device);
    
    return fd;
}




int setup_port(int fd, int speed, int data_bits, int parity, int stop_bits)
{
    int speed_arr[] = {B115200, B9600, B38400, B19200, B4800};
    int name_arr[] = {115200, 9600, 38400, 19200, 4800};
    struct termios opt;
    int ret=-1;
    int i=0;
    int len=0;

    ret = tcgetattr(fd, &opt);                /* get the port attr */
    if (ret < 0)
        printf("Unable to get the attribute\n");

    opt.c_cflag |= (CLOCAL | CREAD); /* enable the receiver, set local mode */
    opt.c_cflag &= ~CSIZE;                        /* mask the character size bits*/

    /* baud rate */
    len = sizeof(speed_arr) / sizeof(int);
    for (i = 0; i < len; i++)
    {
        if (speed == name_arr[i])
        {
            cfsetispeed(&opt, speed_arr[i]);
            cfsetospeed(&opt, speed_arr[i]);
        }
        if (i == len)
            printf("Unsupported baud rate.\n");
    }
    
    /* data bits */
    switch (data_bits)
    {
    case 8:
        opt.c_cflag |= CS8;
        break;
    case 7:
        opt.c_cflag |= CS7;
        break;
    default:
        printf("Unsupported data bits.\n");
    }

    /* parity bits */
    switch (parity)
    {
    case 'N':
    case 'n':
        opt.c_iflag &= ~INPCK;
        opt.c_cflag &= ~PARENB;
        break;
    case 'O':
    case 'o':
        opt.c_iflag |= (INPCK|ISTRIP); /*enable parity check, strip parity bits*/
        opt.c_cflag |= (PARODD | PARENB);
        break;
    case 'E':
    case 'e':
        opt.c_iflag |= (INPCK|ISTRIP); /*enable parity check, strip parity bits*/
        opt.c_cflag |= PARENB;
        opt.c_cflag &= ~PARODD;
        break;
    default:
        printf("Unsupported parity bits.\n");
    }

    /* stop bits */
    switch (stop_bits)
    {
    case 1:
        opt.c_cflag &= ~CSTOPB;
        break;
    case 2:
        opt.c_cflag |= CSTOPB;
        break;
    default:
        printf("Unsupported stop bits.\n");
    }

    /* raw input */
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* raw ouput */
    opt.c_oflag &= ~OPOST;

    //��������
    //RTS/CTS (Ӳ��) ������
    opt.c_cflag &= ~CRTSCTS; //������
    //����� XON/XOFF ������
    opt.c_iflag &= ~IXOFF;//������
    //����� XON/XOFF ������
    opt.c_iflag &= ~IXON ;//������

    // ��ֹ0x0D��0x0A
    opt.c_iflag &= ~(ICRNL|IGNCR);

    tcflush(fd, TCIFLUSH);
    opt.c_cc[VTIME] = 1; /* time out */
    opt.c_cc[VMIN] = 0; /* minimum number of characters to read */

    ret = tcsetattr(fd, TCSANOW, &opt); /* update it now */
    if (ret < 0)
        printf("Unable to setup the port.\n");
    printf("Setup the port OK!\n");

    return 0; /* everything is OK! */
}


void startSerial()
{
    int fd;
    int ret;
    
    fd = open_port("/dev/ttyS0"); /* open the port */
    if (fd < 0)
        exit(0);
	ret = setup_port(fd, 57600, 8, 'N', 1); /* setup the port */
    if (ret<0)
        exit(0);

	serialPort =fd;

	/*
	ret = pthread_create(&write_tid, NULL, write_port_thread, (void*)fd);
    if (ret < 0)
        printf("Create write thread error.");

	*/
	ret = pthread_create(&read_tid, NULL, read_port_thread, NULL);
    if (ret < 0)
        printf("Create read thread error.");
		
	
}
