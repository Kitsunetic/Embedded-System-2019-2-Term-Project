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
    Point v0, v1;
    float m0, m1;
    int i, j;
    Object *o0, *o1;
    
    // Read mouse movement
    read(mouse0.fd, &buf, sizeof(input_event_t));
    
    // Objects hit check
    for(i = 0; i < LEN_OBJECTS-1; i++) {
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
