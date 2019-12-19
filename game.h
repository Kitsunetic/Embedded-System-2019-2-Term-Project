#ifndef __GAME_H__
#define __GAME_H__

#include "glcd.h"

#define LOOK_PLAYER     0x00
#define LOOK_BALL       0x01

/*  */
typedef struct _Object {
    Point pos, v, a;
    byte look;
    bool visible;
    Color color;
} Object;

/*  */
typedef struct _Mouse {
    int fd;
} Mouse;

typedef struct _MouseEvent {
    bool btnLeft, btnRight, btnMiddle;
    Point move;
} MouseEvent;


/*  */
int mouse_init(Mouse* mouse, const char* device_name, Color color);
void mouse_read(Mouse* mouse, MouseEvent* e);

#endif
