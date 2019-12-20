#ifndef __GAME_C__
#define __GAME_C__

#include "game.h"


void stateBegin() {
    if(fb_init(&fb, DEVICE_FB) < 0) {
        perror("FrameBuffer initialize error");
        return -1;
    }
    
    if(mouse_init(&mouse0, DEVICE_MOUSE0, (Color){0, 0, 255}) < 0) {
        perror("Mouse0 initialize error");
        return -1;
    }
    if(mouse_init(&mouse1, DEVICE_MOUSE1, (Color){255, 0, 0}) < 0) {
        perror("Mouse1 initialize error");
        return -1;
    }
    
    state = STATE_INIT;
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
    player1.pos.x = fb.width * 3/4;
    player1.pos.y = fb.height/2;
    player1.look = LOOK_PLAYER;
    player1.visible = true;
    ball.pos.x = fb.width/2;
    ball.pos.y = fb.height/2;
    ball.look = LOOK_BALL;
    ball.visible = true;
    
    state = STATE_PLAYING;
}

void statePlaying() {
    input_event_t buf;
    float dx, dy, l;
    
    // Read mouse movement
    read(mouse0.fd, &buf, sizeof(input_event_t));
    
    // Hit check - radius of players are 2
    // Hit check - player0 : ball
    dx = ball.pos.x - player0.pos.x;
    dy = ball.pos.y - player0.pos.y;
    l = dx*dx + dy*dy;
    if(l < 4.0) {
        
    }
    
    // Change accelerations of players and ball
    
    // Give friction
    
    // Move players
    
    // Move ball
    
    // Show screen
    
    
    state = STATE_GAME_FINISH;
}

void stateGameFinish() {
    state = STATE_PLAYING;
}

void stateGameOver() {
    state = STATE_INIT;
}


int mouse_init(Mouse* mouse, const char* device_name, Color color) {
    if((mouse->fd = open(device_name, O_RDONLY)) < 0) {
        perror("Mouse open error");
        return -1;
    }
    
    return 0;
}

void mouse_read(Mouse* mouse, MouseEvent* e) {
    input_event_t buf;
    byte* p;
    
    read(mouse->fd, &buf, sizeof(input_event_t));
    e->btnLeft = p[0] & 0x01;
    e->btnRight = p[0] & 0x02;
    e->btnMiddle = p[0] & 0x03;
    e->move.x = (int)p[1];
    e->move.y = (int)p[2];
}

#endif
