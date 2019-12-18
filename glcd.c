#ifndef __GLCD_C__
#define __GLCD_C__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include "glcd.h"

int fb_init(fb_dev *dev, const char *fb_name) {
    /* 
    Init framebuffer
    
    - dev->fb_p : mmap pointer
    - dev->width, height: screen size
    - fb_name: commonly "/dev/fb0"
    */
    
    int fd, check;
    struct fb_var_screeninfo st_fvs;
    
    if((fd = open(fb_name, O_RDWR)) < 0) {
        perror("Frame buffer open error");
        return -1;
    }
    
    if((check = ioctl(fd, FBIOGET_VSCREENINFO, st_fvs)) < 0) {
        perror("Frame buffer get information error");
        return -1;
    }
    
    dev->fb_p = (uint32_t*)mmap(0, 
                                st_fvs.xres * st_fvs.yres * 4,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                fd,
                                0);
    close(fd);
    
    if(dev->fb_p == MAP_FAILED) {
        perror("MMAP failed");
        return -1;
    }
    
    dev->width = st_fvs.xres;
    dev->height = st_fvs.yres;
}

uint32_t rgb888to565(byte r, byte g, byte b) {
    /*
    Combine r, g, b into 32bit.
    +---------------------------+
    | 8bit | 8bit | 8bit | 8bit |
    | None | R    | G    | B    |
    +---------------------------+
    */
    uint32_t temp;
    temp = b;
    temp |= g<<8;
    temp |= r<<16;
    return temp;
}

#endif
