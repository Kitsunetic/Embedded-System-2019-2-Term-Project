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

int initAllDevices();
int writeFND(int num);

#endif
