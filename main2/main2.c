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
#include <math.h>
#include <stdint.h>
#include <pthread.h>
#include "circle.h"

#define TIMESCALE           0.1

#define FRAME_BUFFER        "/dev/fb0"
#define DEVICE_MOUSE1       "/dev/input/mouse1"
#define DEVICE_MOUSE2       "/dev/input/mouse2"
#define DEVICE_FND          "/dev/fpga_fnd"

#define STATE_BEGIN         0
#define STATE_INIT          10
#define STATE_PLAYING       20
#define STATE_GAME_FINISH   21
#define STATE_GAMEOVER      22

#define BALL_R              40
#define BALL_M              8
#define PLAYER_R            40
#define PLAYER_M            10

typedef unsigned char byte;
struct FB {
    uint32_t* fb_p;
    unsigned int width;
    unsigned int height;
};
struct Player {
    double x, y, vx, vy, ax, ay;
    uint32_t pixel;
    byte score;
};
struct Ball {
    double x, y, vx, vy, ax, ay;
    uint32_t pixel;
};
struct MouseEvent {
    byte btnLeft, btnRight, btnMiddle;
    double x, y;
};

struct FB fb;
struct Player p1, p2;
struct Ball ball;
int win_player = 0;
int player_fx[] = {1, 3, 5, 6, 7, 8, 8, 9, 9, 10, 10, 10, 9, 9, 8, 8, 7, 6, 5, 3, 1};
int ball_fx[] = {1, 3, 4, 5, 6, 7, 7, 8, 8, 8, 7, 7, 6, 5, 4, 3, 1};

pthread_t t_mouse1, t_mouse2;

int state = STATE_BEGIN;
int mouse1_fd, mouse2_fd;
volatile struct MouseEvent mouse1_e, mouse2_e;
int fnd_fd;


void dswap(double *a, double *b) {
    double temp = *b;
    *b = *a;
    *a = temp;
}

uint32_t makePixel(byte r, byte g, byte b) {
    uint32_t temp;
    temp = b;
    temp |= g<<8;
    temp |= r<<16;
    return temp;
}


void readMouse(int fd, volatile struct MouseEvent* e) {
    struct input_event buf;
    byte* p = (byte*)&buf;
    
    read(fd, &buf, sizeof(struct input_event));
    e->btnLeft = p[0] & 0x01;
    e->btnRight = p[0] & 0x02;
    e->btnMiddle = p[0] & 0x03;
    e->x = p[1] > 0x7F ? (double)p[1]-0x100 : (double)p[1];
    e->y = p[2] > 0x7F ? (double)0x100-p[2] : (double)(-p[2]);
}
void* t_mouse1_entry(void *arg) {
    while(1) readMouse(mouse1_fd, &mouse1_e);
}
void* t_mouse2_entry(void *arg) {
    while(1) readMouse(mouse2_fd, &mouse2_e);
}

void clearScreen(struct FB *fb) {
    int x, y;
    for(y = 0; y < fb->height; y++) {
        for(x = 0; x < fb->width; x++) {
            fb->fb_p[y*fb->width + x] = 0;
        }
    }
}


void stateBegin() {
    int fd;
    struct fb_var_screeninfo st_fvs;
    
    // Load FB
    if((fd = open(FRAME_BUFFER, O_RDWR)) < 0) {
        perror("Frame buffer open error");
        exit(1);
    }
    if(ioctl(fd, FBIOGET_VSCREENINFO, &st_fvs) < 0) {
        perror("Frame buffer get information error");
        exit(1);
    }
    fb.width = st_fvs.xres;
    fb.height = st_fvs.yres;
    printf("Screen size: (%d, %d)\n", fb.width, fb.height);
    fb.fb_p = (unsigned int*)mmap(0, fb.width*fb.height*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if(fb.fb_p == MAP_FAILED) {
        perror("MMAP failed");
        exit(1);
    }
    
    // Load mouse1, 2
    mouse1_fd = open(DEVICE_MOUSE1, O_RDONLY);
    if(mouse1_fd < 0) {
        perror("Mouse1 open error");
        exit(1);
    }
    /* mouse2_fd = open(DEVICE_MOUSE2, O_RDONLY);
    if(mouse2_fd < 0) {
        perror("Mouse2 open error");
        exit(1);
    } */
    
    // Load FND
    fnd_fd = open(DEVICE_FND, O_RDONLY);
    if(fnd_fd < 0) {
        perror("FND open error");
        exit(1);
    }
    
    // Make thread
    pthread_create(&t_mouse1, NULL, t_mouse1_entry, 0);
    pthread_create(&t_mouse2, NULL, t_mouse2_entry, 0);
    
    printf("Load HW complete. Go to state init.\n");
    state = STATE_INIT;
}
void stateInit() {
    p1.x = fb.width * 1/4;
    p1.y = fb.height / 2;
    p1.vx = 0;
    p1.vy = 0;
    p1.ax = 0;
    p1.ay = 0;
    p1.pixel = makePixel(255, 0, 0);
    p2.x = fb.width * 3/4;
    p2.y = fb.height / 2;
    p2.vx = 0;
    p2.vy = 0;
    p2.ax = 0;
    p2.ay = 0;
    p2.pixel = makePixel(0, 0, 255);
    ball.x = fb.width * 1/2;
    ball.y = fb.height / 2;
    ball.vx = 0;
    ball.vy = 0;
    ball.ax = 0;
    ball.ay = 0;
    ball.pixel = makePixel(0, 255, 0);
    
    printf("Initialize complete. Go to state playing.\n");
    state = STATE_PLAYING;
}
void statePlaying() {
    static int clearScreenCounter = 0;
    
    struct input_event buf;
    double dx, dy, dvx, dvy, len, force, theta, m1, m2;
    int i, kx, ky, p1x, p1y, p2x, p2y, bx, by;
    uint32_t p1c, p2c, bc;
    
    // Clear screen
    if(++clearScreenCounter > 1000) {
        clearScreen(&fb);
        clearScreenCounter = 0;
    }
    
    // Goal check
    if(ball.x >= fb.width-BALL_R*3 && fb.height*2/4 <= ball.y && ball.y <= fb.height*3/4) {
        // player0 win
        state = STATE_GAME_FINISH;
        win_player = 1;
        return;
    } else if(ball.x <= BALL_R*3 && fb.height*2/4 <= ball.y && ball.y <= fb.height*3/4) {
        // player1 win
        state = STATE_GAME_FINISH;
        win_player = 2;
        return;
    }
    
    // Object hit check
    // p1:p2
    dx = p1.x-p2.x;
    dy = p1.y-p2.y;
    if(dx*dx + dy*dy <= PLAYER_R*PLAYER_R) {
        dswap(&p1.vx, &p2.vx);
        dswap(&p1.vy, &p2.vy);
    }
    // p1:ball
    dx = p1.x-ball.x;
    dy = p1.y-ball.y;
    if(dx*dx + dy*dy <= BALL_R*BALL_R) {
        /* m1 = (PLAYER_M-BALL_M) / (PLAYER_M+BALL_M);
        m2 = 2*BALL_M / (PLAYER_M+BALL_M);
        dvx = p1.vx*m1 + ball.vx*m2;
        dvy = p1.vy*m1 + ball.vy*m2;
        ball.vx = p1.vx*m2 - ball.vx*m1;
        ball.vy = p1.vy*m2 - ball.vy*m1;
        p1.vx = dvx;
        p1.vy = dvy; */
        dswap(&p1.vx, &ball.vx);
        dswap(&p1.vy, &ball.vy);
    }
    // p2:ball
    dx = p2.x-ball.x;
    dy = p2.y-ball.y;
    if(dx*dx + dy*dy <= BALL_R*BALL_R) {
        /* m1 = (PLAYER_M-BALL_M) / (PLAYER_M+BALL_M);
        m2 = 2*BALL_M / (PLAYER_M+BALL_M);
        dvx = p2.vx*m1 + ball.vx*m2;
        dvy = p2.vy*m1 + ball.vy*m2;
        ball.vx = p2.vx*m2 - ball.vx*m1;
        ball.vy = p2.vy*m2 - ball.vy*m1;
        p2.vx = dvx;
        p2.vy = dvy; */
        dswap(&p2.vx, &ball.vx);
        dswap(&p2.vy, &ball.vy);
    }
    
    // Give friction
    p1.vx   *= 0.999;
    p1.vy   *= 0.999;
    p2.vx   *= 0.999;
    p2.vy   *= 0.999;
    ball.vx *= 0.999;
    ball.vy *= 0.999;
    p1.ax   *= 0.1;
    p1.ay   *= 0.1;
    p2.ax   *= 0.1;
    p2.ay   *= 0.1;
    ball.ax *= 0.1;
    ball.ay *= 0.1;
    
    // Change acceleration using mouse
    p1.ax = mouse1_e.x*0.3;
    p1.ay = mouse1_e.y*0.3;
    p2.ax = mouse2_e.x*0.3;
    p2.ay = mouse2_e.y*0.3;
    mouse1_e.x = 0;
    mouse1_e.y = 0;
    mouse2_e.x = 0;
    mouse2_e.y = 0;
    
    // Change velocity
    p1.vx += TIMESCALE * p1.ax;
    p1.vy += TIMESCALE * p1.ay;
    p2.vx += TIMESCALE * p2.ax;
    p2.vy += TIMESCALE * p2.ay;
    ball.vx += TIMESCALE * ball.ax;
    ball.vy += TIMESCALE * ball.ay;
    
    // Remove player and ball from screen
    p1x = p1.x; p1y = p1.y;
    p2x = p2.x; p2y = p2.y;
    bx = ball.x; by = ball.y;
    for(ky = -PLAYER_R; ky <= PLAYER_R; ky++) {
        for(kx = -PLAYER_R; kx <= PLAYER_R; kx++) {
            if(CIRCLE[(ky+PLAYER_R)*81 + (kx+PLAYER_R)]) {
                fb.fb_p[(p1y-ky)*fb.width + (p1x-kx)] = 0;
                fb.fb_p[(p2y-ky)*fb.width + (p2x-kx)] = 0;
                fb.fb_p[(by-ky)*fb.width + (bx-kx)] = 0;
            }
        }
    }
    
    // Move objects
    p1.x += TIMESCALE * p1.vx;
    p1.y += TIMESCALE * p1.vy;
    p2.x += TIMESCALE * p2.vx;
    p2.y += TIMESCALE * p2.vy;
    ball.x += TIMESCALE * ball.vx;
    ball.y += TIMESCALE * ball.vy;
    
    
    // Wall hit check
    if(p1.x >= fb.width - PLAYER_R*2) {
        p1.vx *= -1;
        p1.ax *= -1;
        p1.x = fb.width - PLAYER_R*2;
    } else if(p1.x <= PLAYER_R*2) {
        p1.vx *= -1;
        p1.ax *= -1;
        p1.x = PLAYER_R*2;
    }
    if(p1.y >= fb.height - PLAYER_R*2) {
        p1.vy *= -1;
        p1.ay *= -1;
        p1.y = fb.height - PLAYER_R*2;
    } else if(p1.y <= PLAYER_R*2) {
        p1.vy *= -1;
        p1.ay *= -1;
        p1.y = PLAYER_R*2;
    }
    if(p2.x >= fb.width - PLAYER_R*2) {
        p2.vx *= -1;
        p2.ax *= -1;
        p2.x = fb.width - PLAYER_R*2;
    } else if(p2.x <= PLAYER_R*2) {
        p2.vx *= -1;
        p2.ax *= -1;
        p2.x = PLAYER_R*2;
    }
    if(p2.y >= fb.height - PLAYER_R*2) {
        p2.vy *= -1;
        p2.ay *= -1;
        p2.y = fb.height - PLAYER_R*2;
    } else if(p2.y <= PLAYER_R*2) {
        p2.vy *= -1;
        p2.ay *= -1;
        p2.y = PLAYER_R*2;
    }
    if(ball.x >= fb.width - PLAYER_R*2) {
        ball.vx *= -1;
        ball.ax *= -1;
        ball.x = fb.width - PLAYER_R*2;
    } else if(ball.x <= PLAYER_R*2) {
        ball.vx *= -1;
        ball.ax *= -1;
        ball.x = PLAYER_R*2;
    }
    if(ball.y >= fb.height - PLAYER_R*2) {
        ball.vy *= -1;
        ball.ay *= -1;
        ball.y = fb.height - PLAYER_R*2;
    } else if(ball.y <= PLAYER_R*2) {
        ball.vy *= -1;
        ball.ay *= -1;
        ball.y = PLAYER_R*2;
    }
    
    // Draw player and ball from screen
    p1x = p1.x; p1y = p1.y;
    p2x = p2.x; p2y = p2.y;
    bx = ball.x; by = ball.y;
    p1c = makePixel(255, 0, 0);
    p2c = makePixel(0, 0, 255);
    bc = makePixel(0, 255, 0);
    for(ky = -PLAYER_R; ky <= PLAYER_R; ky++) {
        for(kx = -PLAYER_R; kx <= PLAYER_R; kx++) {
            if(CIRCLE[(ky+PLAYER_R)*81 + (kx+PLAYER_R)]) {
                fb.fb_p[(p1y-ky)*fb.width + (p1x-kx)] = p1c;
                fb.fb_p[(p2y-ky)*fb.width + (p2x-kx)] = p2c;
                fb.fb_p[(by-ky)*fb.width + (bx-kx)] = bc;
            }
        }
    }
    
    // test
    printf("p1(%f, %f) : [%f, %f] : [%f, %f] : [%f, %f]\n", p1.x, p1.y, p1.vx, p1.vy, p1.ax, p1.ay, mouse1_e.x, mouse1_e.y);
}
void stateGameFinish() {
    
}
void stateGameOver() {
    
}


int main() {
    while(1) {
        switch(state) {
            case STATE_BEGIN:       stateBegin();       break;
            case STATE_INIT:        stateInit();        break;
            case STATE_PLAYING:     statePlaying();     break;
            case STATE_GAME_FINISH: stateGameFinish();  break;
            case STATE_GAMEOVER:    stateGameOver();    break;
        }
    }
    return 0;
}
