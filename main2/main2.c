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

#define FRAME_BUFFER    "/dev/fb0"
#define DEVICE_MOUSE1   "/dev/input/mouse1"
#define DEVICE_MOUSE2   "/dev/input/mouse2"
#define DEVICE_FND      "/dev/fpga_fnd"

#define STATE_BEGIN         0
#define STATE_INIT          10
#define STATE_PLAYING       20
#define STATE_GAME_FINISH   21
#define STATE_GAMEOVER      22

#define BALL_R              8
#define PLAYER_R            10

typedef unsigned char byte;
struct FB {
    uint32_t* fb_p;
    unsigned int width;
    unsigned int height;
};
struct Player {
    double x, y, vx, vy, ax, ay;
    int m;
    uint32_t pixel;
    byte score;
};
struct Ball {
    double x, y, vx, vy, ax, ay;
    int m;
    uint32_t pixel;
};
struct MouseEvent {
    byte btnLeft, btnRight, btnMiddle;
    byte x, y;
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
    e->x = p[1] > 0x7F ? p[1]-0x100 : p[1];
    e->y = p[2] > 0x7F ? p[2]-0x100 : p[2];
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
    mouse2_fd = open(DEVICE_MOUSE2, O_RDONLY);
    if(mouse2_fd < 0) {
        perror("Mouse2 open error");
        exit(1);
    }
    
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
    p1.m = 10;
    p1.vx = 0;
    p1.vy = 0;
    p1.ax = 0;
    p1.ay = 0;
    p1.pixel = makePixel(255, 0, 0);
    p2.x = fb.width * 3/4;
    p2.y = fb.height / 2;
    p2.m = 10;
    p2.vx = 0;
    p2.vy = 0;
    p2.ax = 0;
    p2.ay = 0;
    p2.pixel = makePixel(0, 0, 255);
    ball.x = fb.width * 1/2;
    ball.y = fb.height / 2;
    ball.m = 10;
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
    double dx, dy, dvx, dvy, len, force, theta;
    int i;
    
    // test
    printf("%d:%d, %d:%d\n", mouse1_e.x, mouse1_e.y, mouse2_e.x, mouse2_e.y);
    
    // Clear screen
    if(++clearScreenCounter > 10) {
        clearScreen(&fb);
        clearScreenCounter = 0;
    }
    
    // Goal check
    if(ball.x >= fb.width-BALL_R && fb.height/3 <= ball.y && ball.y <= fb.height*2/3) {
        // player0 win
        state = STATE_GAME_FINISH;
        win_player = 1;
        return;
    } else if(ball.x <= BALL_R && fb.height/3 <= ball.y && ball.y <= fb.height*2/3) {
        // player1 win
        state = STATE_GAME_FINISH;
        win_player = 2;
        return;
    }
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
