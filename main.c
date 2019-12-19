#include "game.h"

#define DEVICE_FB       "/dev/fb0"
// "/dev/input/mouse0" is touch screen.
#define DEVICE_MOUSE0   "/dev/input/mouse1"
#define DEVICE_MOUSE1   "/dev/input/mouse2"

#define STATE_BEGIN         0x00
#define STATE_INIT          0x10
#define STATE_PLAYING       0x20
#define STATE_GAME_FINISH   0x21
#define STATE_GAMEOVER      0x22
byte state = STATE_BEGIN;

// Frame buffer
fb_dev fb;

// Mouse
Mouse mouse0, mouse1;
Color mouse0_color = {255, 0, 0}, mouse1_color = {0, 0, 255};


int main() {
    state = STATE_BEGIN;
    
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
        return -1;
    }
    
    if(mouse_init(&mouse0, DEVICE_MOUSE0, mouse0_color) < 0) {
        perror("Mouse0 initialize error");
        return -1;
    }
    if(mouse_init(&mouse1, DEVICE_MOUSE1, mouse1_color) < 0) {
        perror("Mouse1 initialize error");
        return -1;
    }
    
    state = STATE_INIT;
}

void stateInit() {
    
    
    state = STATE_PLAYING;
}

void statePlaying() {
    
    
    state = STATE_GAME_FINISH;
}

void stateGameFinish() {
    state = STATE_PLAYING;
}

void stateGameOver() {
    state = STATE_INIT;
}
