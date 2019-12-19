#ifndef __GLCD_H__
#define __GLCD_H__

#include <stdint.h>
#include "commontype.h"

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
    int x, y;
} Point;

typedef struct _Mouse {
    int fd;
    unsigned int x, y;
    Color color;
} Mouse;

typedef struct _fb_dev {
    uint32_t* fb_p;
    int width;
    int height;
} fb_dev;


/* Initialize frame buffer */
int fb_init(fb_dev* dev, const char* fb_name);

/* Convert color(RGB) into 32bit pixel */
uint32_t makePixel(Color color);

/* Draw rectengular on screen */
void makeRect(fb_dev* fb, Color color, Point left, Point right);

/* Clear screen black */
void clearScreen(fb_dev* fb);

/* Draw dot on screen */
void draw(fb_dev* fb, Color color, Point point);


/*  */
int mouse_init(Mouse* mouse, const char* device_name, Color color);


/*  */
void releaseBitmap(Image* image);

/*  */
Image* readBitmap(char* filename, char opt);

/*  */
void drawBitmap(fb_dev* fb, Image* bmp, int xo, int yo, int alpha);

#endif
