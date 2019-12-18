#ifndef __GLCD_H__
#define __GLCD_H__

#include <stdint.h>
#include "commontype.h"

typedef struct _fb_dev {
    uint32_t* fb_p;
    int width;
    int height;
} fb_dev;

int fb_init(fb_dev *dev, const char *fb_name);
uint32_t rgb888to565(byte r, byte g, byte b);

#endif
