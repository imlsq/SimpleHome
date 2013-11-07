
void *read_port_thread(void *argc);
void *write_port_thread(void *argc);
int open_port(const char* device);
int setup_port(int fd, int speed, int data_bits, int parity, int stop_bits);
void setupSerial();



