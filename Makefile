CC = arm-linux-gnueabihf-gcc
CFLAGS = -c
LDFLAGS = -lpthread -lrt -lm
TARGET = hockey.out

all: main.o fpga.o glcd.o commontype.o
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) $^

glcd.o: glcd.c
	$(CC) $(CFLAGS) $^
	
fpga.o: fpga.c
	$(CC) $(CFLAGS) $^
	
commontype.o: commontype.c
	$(CC) $(CFLAGS) $^

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .interface*
	rm -rf .tmp*
	rm -rf *.gch
