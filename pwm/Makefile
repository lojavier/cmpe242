CC=arm-linux-gnueabihf-gcc
RM=rm -f

obj-m += pwm.o

all: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) -Wall -o pwm_test pwm_test.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) pwm_test
