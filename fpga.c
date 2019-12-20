#ifndef __FPGA_C__
#define __FPGA_C__

#include "fpga.h"

int initDevice(int *dev, const char *device) {
    if((*dev = open(device, O_RDWR)) < 0) {
        perror("Cannot open FND device");
        return -1;
    }
    return 0;
}

int initAllDevices() {
    printf("Open device %s\n", DEVICE_FND);
    if(initDevice(&dev.fnd, DEVICE_FND) < 0) return -1;
}

int writeFND(int num) {
    byte ret, data[4];

    data[0] = num/1000;
    num %= 1000;
    data[1] = num/100;
    num %= 100;
    data[2] = num/10;
    num %= 10;
    data[3] = num;

    if((ret = write(dev.fnd, &data, 4)) < 0) {
        perror("FND write error");
        return -1;
    }
    return 0;
}

#endif
