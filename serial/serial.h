#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#define SERIAL_PORT_DEFAULT     "/dev/ttyAMA0"
#define BAUD_RATE_DEFAULT       115200

extern int ser_init(const char* device, int speed);
extern void ser_send(int fd, const void *data, int len);
extern int ser_recieve(int fd, uint8_t *str_dat, int n);

#ifdef __cplusplus
}
#endif

#endif

