CC = arm-linux-gnueabihf-gcc
CFLAGS = -c
LDFLAGS = -lpthread -lrt
TARGET = hockey.out

app: main.o glcd.o
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) $^

glcd.o: glcd.c
	$(CC) $(CFLAGS) $^

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .interface*
	rm -rf .tmp*
