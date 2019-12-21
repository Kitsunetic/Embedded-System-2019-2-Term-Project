#ifndef __GLCD_H__
#define __GLCD_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/fb.h>

#include "commontype.h"

typedef struct input_event input_event_t;

typedef struct _BITMAP {
	unsigned short BM;
	unsigned int filesize;
	unsigned short prog1;
	unsigned short prog2;
	unsigned int offset;
	unsigned int header_size;
	unsigned int width;
	unsigned int height;
	unsigned short num_of_pallet;
	unsigned short depth;
	unsigned int comp;
	unsigned int size;
	unsigned int width_res;
	unsigned int height_res;
	unsigned int pallet_color;
	unsigned int imp_color;	
} Bitmap;

typedef struct _IMAGE {
	int width;
	int height;
	unsigned int *image;
} Image;

typedef struct _Color {
    byte r, g, b;
} Color;

typedef struct _Point {
    double x, y;
} Point;

typedef struct _fb_dev {
    uint32_t* fb_p;
    int width;
    int height;
} fb_dev;


/* Initialize frame buffer */
int fb_init(fb_dev *dev, const char *fb_name);

/* Convert color(RGB) into 32bit pixel */
uint32_t makePixel(Color color);

/* Draw rectengular on screen */
void makeRect(fb_dev *fb, Color color, Point left, Point right);

/* Clear screen black */
void clearScreen(fb_dev *fb);

/* Draw dot on screen */
void draw(fb_dev *fb, Color color, Point point);


/*  */
void releaseBitmap(Image *image);

/*  */
Image* readBitmap(const char *filename);

/*  */
void drawBitmap(fb_dev *fb, Image *bmp, int xo, int yo, int alpha);

void drawPlayer(fb_dev* fb, Point p, Color c);
void drawBall(fb_dev* fb, Point p, Color c);

#endif
