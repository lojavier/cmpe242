CC=arm-linux-gnueabihf-gcc
CXX=arm-linux-gnueabihf-g++
RM=rm -f

obj-m += adc.o

all: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CXX) -Wall -o adc_test mcp3008Spi.cpp adc_test.cpp

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) adc_test
