/* 
/dev/input/mouse0 이 어떻게 출력을 내는지 살펴본다.

PC에서 테스트시:
    좌 - x=255
    우 - x=1
    상 - y=1
    하 - y=255

Example...
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 2, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 2, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 2, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 2, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 2, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 2, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 2, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 2, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 2, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 2, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 1, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 0, Y: 255, L: 0, R: 0, M: 0
X: 1, Y: 0, L: 0, R: 0, M: 0
X: 0, Y: 1, L: 0, R: 0, M: 0
X: 0, Y: 0, L: 1, R: 0, M: 0
X: 0, Y: 0, L: 0, R: 0, M: 0
 */
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

typedef struct input_event input_event_t;
typedef unsigned char byte;

int main() {
    input_event_t buf;
    byte* p;
    int fd;
    byte btnLeft, btnRight, btnMiddle;
    int posX, posY;
    
    
    fd = open("/dev/input/mouse1", O_RDONLY);
    p = (byte*)&buf;
    while(1) {
        read(fd, &buf, sizeof(input_event_t));
        btnLeft = p[0] & 0x01;
        btnRight = p[0] & 0x02;
        btnMiddle = p[0] & 0x04;
        posX = p[1] > 0x7F ? (int)p[1]-0x100 : (int)p[1];
        posY = p[2] > 0x7F ? (int)p[2]-0x100 : (int)p[2];
        printf("X: %d, Y: %d, L: %d, R: %d, M: %d\n", posX, posY, btnLeft, btnRight, btnMiddle);
    }
}
