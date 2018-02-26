CC=arm-linux-gnueabihf-gcc
RM=rm -f

obj-m += led.o

all: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) -Wall -o led_test led_test.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) led_test
