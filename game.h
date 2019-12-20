#ifndef __GAME_H__
#define __GAME_H__

#include "glcd.h"

#define LOOK_PLAYER     0x00
#define LOOK_BALL       0x01

#define DEVICE_FB       "/dev/fb0"
// "/dev/input/mouse0" is touch screen.
#define DEVICE_MOUSE0   "/dev/input/mouse1"
#define DEVICE_MOUSE1   "/dev/input/mouse2"

/*  */
typedef struct _Object {
    Point pos, v, a;
    float m, r;
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

// Frame buffer
fb_dev fb;

// Mouses
Mouse mouse0, mouse1;

// State
#define STATE_BEGIN         0x00
#define STATE_INIT          0x10
#define STATE_PLAYING       0x20
#define STATE_GAME_FINISH   0x21
#define STATE_GAMEOVER      0x22
void stateBegin();
void stateInit();
void statePlaying();
void stateGameFinish();
void stateGameOver();
byte state = STATE_BEGIN;

// Objects
Object player0, player1;
Object ball;
Object* objects[] = {&player0, &player1, &ball};
#define LEN_OBJECTS 3

// Friction
float friction;

/*  */
int mouse_init(Mouse* mouse, const char* device_name, Color color);
void mouse_read(Mouse* mouse, MouseEvent* e);

#endif
