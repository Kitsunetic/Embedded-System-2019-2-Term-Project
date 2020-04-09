CC = arm-linux-gnueabihf-gcc
CFLAGS = -c
LDFLAGS = -lpthread -lrt -lm
TARGET = hockey.out

all: main2.o
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

main.o: main2.c
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
	rm -rf *.out
