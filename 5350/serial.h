

void *read_port_thread();
void write_serial_port(char *buf);
const char *syn_write_serial(char *buf, int timeout);
int open_port(const char* device);
int setup_port(int fd, int speed, int data_bits, int parity, int stop_bits);
void startSerial();






