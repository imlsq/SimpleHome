#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <termios.h>



char *receive="";
char *buf = "Are you going to die? \r\n";
pthread_t read_tid; 
pthread_t write_tid;


void *read_port_thread(void *argc)
{
    int num;
    char tmp[1];
	int fd;
    
    fd = (int)argc;
    while(1)
    {
        while ((num=read(fd, tmp, 1))>0)
        {            
            tmp[num+1] = '\0';
			receive=tmp;
            printf("%s",receive);
			
        }
		sleep(1);
        if (num < 0) 
            pthread_exit(NULL);
     }
	 pthread_exit(NULL);
     
}


void *write_port_thread(void *argc)
{
    int ret;
    int fd;
    static int cnt = 1;
    char send_buf[512] = {0};

    fd = (int)argc;

    while (1)
    {
        sprintf(send_buf, "%d %s", cnt, buf);
        printf("writing time %d... ", cnt++);

        ret = write(fd, send_buf, strlen(send_buf));

        if (ret < 0)
            pthread_exit(NULL);
        printf("write num---: %d\n", ret);
        sleep(20);
    }
    pthread_exit(NULL);
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

    //设置流控
    //RTS/CTS (硬件) 流控制
    opt.c_cflag &= ~CRTSCTS; //无流控
    //输入的 XON/XOFF 流控制
    opt.c_iflag &= ~IXOFF;//不启用
    //输出的 XON/XOFF 流控制
    opt.c_iflag &= ~IXON ;//不启用

    // 防止0x0D变0x0A
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

	
	ret = pthread_create(&write_tid, NULL, write_port_thread, (void*)fd);
    if (ret < 0)
        printf("Create write thread error.");

	
	ret = pthread_create(&read_tid, NULL, read_port_thread, (void*)fd);
    if (ret < 0)
        printf("Create read thread error.");
	
}
