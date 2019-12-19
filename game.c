#ifndef __GAME_C__
#define __GAME_C__

#include "game.h"




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
