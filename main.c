#include <math.h>

#include "glcd.h"
#include "fpga.h"

#define LOOK_PLAYER     0x00
#define LOOK_BALL       0x01

#define DEVICE_FB       "/dev/fb0"
// "/dev/input/mouse0" is touch screen.
#define DEVICE_MOUSE0   "/dev/input/mouse1"
#define DEVICE_MOUSE1   "/dev/input/mouse2"

/*  */
typedef struct _Object {
    Point pos, v, a;
    double m, r;
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
    volatile Point move;
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

byte winPlayer;
int score0, score1;

// Friction, gravity acceleration
#define FRICTION    0.3
#define GRAVITY     9.8
#define TIMESCALE   0.1

byte screenClearCount = 0;

/*  */
int mouseInit(Mouse* mouse, const char* device_name, Color color);
void mouseRead(Mouse* mouse, MouseEvent* e);

/* Give sign of value v */
double sign(double v);

/* Draw object to screen */
void drawObject(fb_dev *fb, Object* obj);

int main() {
    while(1) {
        switch(state) {
            case STATE_BEGIN:       stateBegin();       break;
            case STATE_INIT:        stateInit();        break;
            case STATE_PLAYING:     statePlaying();     break;
            case STATE_GAME_FINISH: stateGameFinish();  break;
            case STATE_GAMEOVER:    stateGameOver();    break;
            default:
                printf("Unknown state(0x%02x) error\n", state);
                return -1;
        }
    }
    return 0;
}


void stateBegin() {
    if(fb_init(&fb, DEVICE_FB) < 0) {
        perror("FrameBuffer initialize error");
        return;
    }
    
    if(mouseInit(&mouse0, DEVICE_MOUSE0, (Color){0, 0, 255}) < 0) {
        perror("Mouse0 initialize error");
        return;
    }
    if(mouseInit(&mouse1, DEVICE_MOUSE1, (Color){255, 0, 0}) < 0) {
        perror("Mouse1 initialize error");
        return;
    }

    // Initialize all FPGA devices
    initAllDevices();

    // Initialize state
    state = STATE_INIT;
    
    printf("Hardware setup complete...\n");
}

void stateInit() {
    // Initialize position and velocity, acceleration of players and ball
    memset(&player0, 0x00, sizeof(player0));
    memset(&player1, 0x00, sizeof(player1));
    memset(&ball, 0x00, sizeof(ball));
    player0.pos.x = fb.width * 1/4;
    player0.pos.y = fb.height/2;
    player0.look = LOOK_PLAYER;
    player0.visible = true;
    player0.m = 10;
    player0.color = (Color){255, 0, 0};
    player1.pos.x = fb.width * 3/4;
    player1.pos.y = fb.height/2;
    player1.look = LOOK_PLAYER;
    player1.visible = true;
    player1.m = 10;
    player1.color = (Color){0, 0, 255};
    ball.pos.x = fb.width/2;
    ball.pos.y = fb.height/2;
    ball.look = LOOK_BALL;
    ball.visible = true;
    ball.m = 5;
    ball.color = (Color){0, 255, 0};
    
    ball.v.x = 1;
    
    score0 = 0;
    score1 = 0;

    state = STATE_PLAYING;
    
    clearScreen(&fb);
}

void statePlaying() {
    input_event_t buf;
    double dx, dy, l, f, theta;
    Point v0, v1;
    double m0, m1;
    int i, j;
    Object *o0, *o1, *o;
    MouseEvent e0, e1;
    
    // Clear screen
    /* if(screenClearCount++ > 60) {
        printf("clear: %d\n", screenClearCount);
        clearScreen(&fb);
        screenClearCount = 0;
    } */
    
    // Goal check
    if(ball.pos.x >= fb.width-ball.r && fb.height/3 <= ball.pos.y && ball.pos.y <= fb.height*2/3) {
        // player0 win
        state = STATE_GAME_FINISH;
        winPlayer = 0;
        return;
    } else if(ball.pos.x <= ball.r && fb.height/3 <= ball.pos.y && ball.pos.y <= fb.height*2/3) {
        // player1 win
        state = STATE_GAME_FINISH;
        winPlayer = 1;
        return;
    }

    // Objects hit check
    /* for(i = 0; i < LEN_OBJECTS-1; i++) {
        for(j = i+1; j < LEN_OBJECTS; j++) {
            o0 = objects[i];
            o1 = objects[j];
            dx = o0->pos.x - o1->pos.x;
            dy = o0->pos.y - o1->pos.y;
            l = o0->r + o1->r;
            if(dx*dx + dy*dy < l*l) {
                m0 = (o0->m-o1->m)/(o0->m+o1->m);
                m1 = 2*o1->m/(o0->m+o1->m);
                v0.x = o0->v.x*m0 + o1->v.x*m1;
                v0.y = o0->v.y*m0 + o1->v.y*m1;
                v1.x = o0->v.x*m1 - o1->v.x*m0;
                v1.y = o0->v.y*m1 - o1->v.y*m0;
                o0->v = v0;
                o1->v = v1;
            }
        }
    } */

    // Give friction
    printf("here\n");
    for(i = 0; i < LEN_OBJECTS; i++) {
        o = objects[i];
        if(o->v.x <= 0.05 && o->v.y <= 0.05) {
            f = o->m*GRAVITY*FRICTION;
            theta = atan(o->v.y/o->v.x);
            dx = f*cos(theta);
            dy = f*sin(theta);
            o->a.x = -sign(o->v.x) * dx;
            o->a.y = -sign(o->v.y) * dy;
        }
    }
    printf("%f, %f\n", o->pos.x, o->pos.y);
    
    // Read mouse movement
    mouseRead(&mouse0, &e0);
    mouseRead(&mouse1, &e1);
    printf("herex\n");
    
    // Change accelerations of objects
    player0.a.x += e0.move.x;
    player0.a.y += e0.move.y;
    player1.a.x += e1.move.x;
    player1.a.y += e1.move.y;

    // Move objects
    for(i = 0; i < LEN_OBJECTS; i++) {
        o = objects[i];
        
        // Remove old object from screen
        Color color_temp = o->color;
        o->color = (Color){0, 0, 0};
        drawObject(&fb, o);
        o->color = color_temp;
        
        // Change velocity
        o->v.x += o->a.x;
        o->v.y += o->a.y;
        
        /* // Move position
        o->pos.x += o->v.x * TIMESCALE;
        o->pos.y += o->v.y * TIMESCALE;
        
        // Wall hit check
        if(o->pos.x + o->r >= fb.width) {
            // hit right
            o->pos.x = 2*fb.width - o->pos.x - o->r;
            o->v.x *= -1;
        } else if(o->pos.x - o->r <= 0) {
            // hit left
            o->pos.x = o->r - o->pos.x;
            o->v.x *= -1;
        } else if(o->pos.y + o->r >= fb.height) {
            // hit top
            o->pos.y = 2*fb.height - o->pos.y - o->r;
            o->v.y *= -1;
        } else if(o->pos.y - o->r <= 0) {
            // hit bottom
            o->pos.y = o->r - o->pos.y;
            o->v.y *= -1;
        } */

        // Draw new object from screen
        drawObject(&fb, o);
    }
}

void stateGameFinish() {
    // Count score
    if(winPlayer == 0) score0++;
    else if(winPlayer == 1) score1++;

    // Update FND
    writeFND(score0 * 100 + score1);

    // Check game over
    if(score0 + score1 >= 5) {
        state = STATE_GAMEOVER;
    } else {
        state = STATE_PLAYING;
    }
    
    clearScreen(&fb);
}

void stateGameOver() {
    // Show game over screen
    
    state = STATE_INIT;
}


int mouseInit(Mouse* mouse, const char* device_name, Color color) {
    if((mouse->fd = open(device_name, O_RDONLY)) < 0) {
        perror("Mouse open error");
        return -1;
    }
    
    return 0;
}

void mouseRead(Mouse* mouse, MouseEvent* e) {
    input_event_t buf;
    byte* p;
    
    read(mouse->fd, &buf, sizeof(input_event_t));
    e->btnLeft = p[0] & 0x01;
    e->btnRight = p[0] & 0x02;
    e->btnMiddle = p[0] & 0x03;
    e->move.x = p[1] > 0x7F ? p[1]-0x100 : p[1];
    e->move.y = p[2] > 0x7F ? p[2]-0x100 : p[2];
}


double sign(double v) {
    if      (v >= 0.05)  return  1.0;
    else if (v == -0.05) return  0.0;
    else                 return -1.0;
}


void drawObject(fb_dev *fb, Object *obj) {
    switch(obj->look) {
        case LOOK_PLAYER: drawPlayer(fb, &obj->pos, &obj->color); break;
        case LOOK_BALL: drawBall(fb, &obj->pos, &obj->color); break;
    }
}
