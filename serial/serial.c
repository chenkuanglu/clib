// serial driver
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>  
#include <pthread.h>  
#include <errno.h>  

#include "serial.h"
#include "cstr.h"

#ifdef __cplusplus
extern "C"{
#endif

int speed_arr[] = { 
    B500000, B115200,B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300
};

int name_arr[] = {
    500000, 115200, 57600,38400, 19200, 9600, 4800, 2400, 1200, 300
};

static int set_speed(int fd, int speed)
{
    int status = -1;
    struct termios opt;
    
    if (tcgetattr(fd, &opt) !=  0) {
        printfd(CCL_RED "Cannot get serial attr" CCL_END);
      	return -1;
    }
    for (unsigned i = 0;  i < sizeof(speed_arr)/sizeof(int); i++) {
        if (speed == name_arr[i]) {
            cfsetispeed(&opt, speed);
            cfsetospeed(&opt, speed);
            status = tcsetattr(fd, TCSANOW, &opt);
            if  (status != 0) {
                printfd(CCL_RED "Cannot set serial speed" CCL_END);
            }
            tcflush(fd, TCIOFLUSH);
            break;
     	}
    }

    return status;
}


static int set_mode(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    
    if (tcgetattr(fd,&options) !=  0) {
        printfd(CCL_RED "Cannot get serial attr" CCL_END);
      	return -1;
    }
    // input mode
    options.c_iflag &= ~(ICRNL | INLCR); 
    options.c_iflag &= ~(IXON | IXOFF | IXANY); 
    // output mode
    options.c_oflag &= ~OPOST; 
    // control mode
	options.c_cflag |= CBAUDEX;
  	options.c_cflag &= ~CSIZE;
    options.c_cflag |= (CLOCAL | CREAD); 
    options.c_cflag &= ~CRTSCTS;            // disable hardware cts/rts
    options.c_cflag &= ~(ONLCR | OCRNL); 
    // local mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 

    // data bits
    switch (databits) {
        case 5:
            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            options.c_cflag |= CS8;
            printfd(CCL_RED "Unsupported serial data-bits: %d" CCL_END, databits);
            return -1;
    }

    // parity
    switch (parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;     /* Clear parity enable */
            options.c_iflag &= ~INPCK;      /* Disable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);   /* odd */ 
            options.c_iflag |= INPCK;               /* Enable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;      /* Enable parity */
            options.c_cflag &= ~PARODD;     /* even */  
            options.c_iflag |= INPCK;       /* Enable parity checking */
            break;
        case 'S':
        case 's':                           /* as no parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            printfd(CCL_RED "Unsupported serial parity: %c" CCL_END, parity);
            return -1;
    }
 
    switch (stopbits) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            printfd(CCL_RED "Unsupported serial stop-bits: %d" CCL_END, stopbits);
            return -1;
    }
	
    // control char, only have effect on read
    options.c_cc[VTIME] = 30;       // block 3s = 30 * 100ms
    options.c_cc[VMIN] = 0;         // read(fd, buf, size), VMIN = min(size, VMIN)

    /* Update the options and do it NOW */
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        printfd(CCL_RED "Cannot set serial option: %s" CCL_END, strerror(errno));
		return -1;
	}
    tcflush(fd, TCIFLUSH);
	
    return 0;
}
 
int serial_init(const char* dev, int speed)
{
    // not block, 0 returned if there have no data received
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd > 0) {
    	if (set_mode(fd, 8, 1, 'N') < 0) {
            return -1;
    	}
	    set_speed(fd, speed);
	}
	return fd;
}

void ser_send(int fd, const void *data, int len)
{
	int ret, sum = 0;
	
	if (len > 0) {
		do {
			ret = write(fd, data, len);
			if (ret >= 0) {
			    sum += ret;
			} else {
			    loge("fail to send serial data: %s", strerror(errno));
			    break;
			}
		} while (len != sum);
	}
}

int ser_recieve(int fd, uint8_t *str_dat, int n)
{
    int nread;
    nread = read(fd, str_dat, n);
    
    if (nread < 0) {
        loge("fail to receive serial data: %s", strerror(errno));
        return 0;
    }
    
    return nread;
}


#ifdef __cplusplus
}
#endif

