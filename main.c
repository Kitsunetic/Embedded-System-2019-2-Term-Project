#include "game.h"

#define DEVICE_FB       "/dev/fb0"
// "/dev/input/mouse0" is touch screen.
#define DEVICE_MOUSE0   "/dev/input/mouse1"
#define DEVICE_MOUSE1   "/dev/input/mouse2"

// states
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

// Frame buffer
fb_dev fb;

// Mouses
Mouse mouse0, mouse1;

// Players
Object player0, player1;

// Ball
Object ball;

// Friction
float friction;

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
    
    // Read mouse movement
    read(mouse0.fd, &buf, sizeof(input_event_t));
    
    // Hit check
    
    // Change accelerations of players and ball
    
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
