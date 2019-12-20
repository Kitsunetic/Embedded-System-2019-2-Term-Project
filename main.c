#include "game.h"

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
