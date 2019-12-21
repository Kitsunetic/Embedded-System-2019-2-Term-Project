#ifndef __GLCD_C__
#define __GLCD_C__

#include "glcd.h"

int fb_init(fb_dev* dev, const char* fb_name) {
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

uint32_t makePixel(Color color) {
    /*
    Combine r, g, b into 32bit.
    +---------------------------+
    | 8bit | 8bit | 8bit | 8bit |
    | None | R    | G    | B    |
    +---------------------------+
    */
    uint32_t temp;
    temp = color.b;
    temp |= color.g<<8;
    temp |= color.r<<16;
    return temp;
}

void makeRect(fb_dev* fb, Color color, Point left, Point right) {
	unsigned int pixel;
	int offset;
	int repx, repy;

	pixel = makePixel(color);
	for(repy = left.y; repy <= right.y; repy++){
		offset = repy * fb->width;
		for(repx = left.x; repx <= right.x; repx++) {
			fb->fb_p[offset+repx] = pixel;
		}
	}
}

void clearScreen(fb_dev* fb) {
    int x, y;
    Color color = {.r = 0, .g = 0, .b = 0};
    uint32_t pixel = makePixel(color);
    
    for(y = 0; y < fb->height; y++) {
        for(x = 0; x < fb->width; x++) {
            fb->fb_p[y*fb->width + x] = pixel;
        }
    }
}

void draw(fb_dev* fb, Color color, Point point) {
    uint32_t pixel = makePixel(color);
    fb->fb_p[(int)(point.x + point.y*fb->width)] = pixel;
}


void releaseBitmap(Image* image) {
    free(image->image);
    free(image);
}

Image* readBitmap(const char *filename) {
    FILE *fp;
    unsigned int *bmp, *temp1, *temp2;
    int resval;
    int x, y;
    Bitmap bitmap;
    Image *retval;
    
    fp = fopen(filename, "r");
    if(fp < 0) {
        perror("Bitmap file open failed.");
        return (Image*)-1;
    }
    
    fread(&bitmap, sizeof(Bitmap), 1, fp);
    fseek(fp, bitmap.offset, SEEK_SET);
    
    retval = (Image*)malloc(sizeof(Image));
    
    temp1 = (unsigned int*)malloc(bitmap.width*bitmap.height*bitmap.depth/8);
    temp2 = (unsigned int*)malloc(bitmap.width*bitmap.height*bitmap.depth/8);
    retval->image = (unsigned int*)malloc(bitmap.width*bitmap.height*bitmap.depth/8);
    resval = fread(temp1, sizeof(unsigned int), bitmap.width*bitmap.height, fp);
    
    for(x = 0; x < bitmap.width; x++) {
        for(y = 0; y < bitmap.height; y++) {
            temp2[x + bitmap.width*y] = temp1[bitmap.width - x + bitmap.width*y];
        }
    }
    for(x = 0; x < bitmap.width*bitmap.height; x++) {
        retval->image[x] = temp2[bitmap.width*bitmap.height - x];
    }
    
    free(temp1);
    free(temp2);
    fclose(fp);
    
    retval->width = bitmap.width;
    retval->height = bitmap.height;
    return retval;
}

void drawBitmap(fb_dev* fb, Image* bmp, int xo, int yo, int alpha) {
    int x, y;
    for(y = 0; y < bmp->height; y++) {
        for(x = 0; x < bmp->width; x++) {
            if      (x+xo > fb->width)  break;
            else if (y+yo > fb->height) break;
            else {
                fb->fb_p[(x+xo) + (y+yo)*(fb->width)] = bmp->image[x + y*(bmp->width)];
            }
        }
    }
}

void drawPlayer(fb_dev* fb, Point p, Color c) {
    // radius = 10
    unsigned int pixel = makePixel(c);
    int i;
    int x = p.x, y = p.y;
    int dx, dy, fx[] = {1, 3, 5, 6, 7, 8, 8, 9, 9, 10, 10, 10, 9, 9, 8, 8, 7, 6, 5, 3, 1};
    
    for(dy = -10; dy <= 10; dy++) {
        for(dx = -fx[dy+10]; dx <= fx[dy+10]; dx++) {
            fb->fb_p[(y+dy)*fb->width + (x+dx)] = pixel;
        }
    }
}

void drawBall(fb_dev *fb, Point p, Color c) {
    // radius = 8
    unsigned int pixel = makePixel(c);
    int i;
    int x = p.x, y = p.y;
    int dx, dy, fx[] = {1, 3, 4, 5, 6, 7, 7, 8, 8, 8, 7, 7, 6, 5, 4, 3, 1};
    
    for(dy = -8; dy <= 8; dy++) {
        for(dx = -fx[dy+8]; dx <= fx[dy+8]; dx++) {
            fb->fb_p[(y+dy)*fb->width + (x+dx)] = pixel;
        }
    }
}

#endif
