#ifndef __FPGA_H__
#define __FPGA_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include "commontype.h"

#define DEVICE_FND "/dev/fpga_fnd"

/* Devices */
struct _dev {
    int fnd;
} dev;

int initDevice(int *dev, const char *device);
int initAllDevices();
int writeFND(int num);

#endif
