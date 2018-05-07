// http://www.hertaville.com/interfacing-an-spi-adc-mcp3008-chip-to-the-raspberry-pi-using-c.html
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "adc.h"
#include "mcp3008Spi.h"

using namespace std;
 
int main(int argc, char **argv)
{
	int fd;
	double inputVoltage = 3.3;
	double outputVoltage = 0.0;
	double period = 0;
	double countDuration;
	double frequency = 0;
	double prevFreq = -1;
	unsigned int divisor = 0;
	int a2dVal = 0;
	int a2dChannel = 0;
	unsigned char data[3];
	mcp3008Spi a2d("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);

	struct pwm_args
    {
        unsigned int mode;
        unsigned int rotation;
        unsigned int divisor;
        unsigned int duty_cycle;
    };

    struct pwm_args pwm0;
    pwm0.mode = PWMMODE;
    pwm0.rotation = 0;
    pwm0.divisor = 1;
    pwm0.duty_cycle = 50;

    for(int i = 0; i < argc; ++i)
	{
		if(strcmp(argv[i],"-m") == 0)
		{
			pwm0.mode = (unsigned int)atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-r") == 0)
		{
			pwm0.rotation = (unsigned int)atoi(argv[i+1]);
			i++;
		}
	}

	fd = open("/dev/pwm", O_RDWR);
	if(fd < 0)
	{
		perror("Failed to open /dev/pwm");
		return errno;
	}

	while(1)
    {
    	usleep(500000);

        data[0] = 1;
        data[1] = 0b10000000 |( ((a2dChannel & 7) << 4));

        a2d.spiWriteRead(data, sizeof(data));

        a2dVal = 0;
        a2dVal = (data[1] << 8) & 0b1100000000;
        a2dVal |= (data[2] & 0xff);
        
        outputVoltage = inputVoltage * (a2dVal/(double)1023);
        printf("ADC Value = %d || Output Voltage = %.2f\n", a2dVal, outputVoltage);

        if (a2dVal > 1000) {
        	frequency = 5000;
        } else if (a2dVal > 800) {
        	frequency = 4000;
        } else if(a2dVal > 600) {
        	frequency = 3000;
        } else if(a2dVal > 400) {
        	frequency = 2000;
        } else if(a2dVal > 200) {
        	frequency = 1000;
        } else if(a2dVal > 20) {
        	frequency = 500;
        } else {
        	frequency = 0;
        }

        if(frequency != prevFreq)
        {
        	prevFreq = frequency;
        	period = 1.0 / frequency;
			countDuration = period / (counts*1.0f);
			divisor = (unsigned int)(19200000.0f / (1.0/countDuration));
			if( divisor < 0 || divisor > 4095 )
			{
				divisor = 0;
			}
        	pwm0.divisor = divisor;
	        if(ioctl(fd, ENABLE_PWM, &pwm0) != 0)
			{
				printf("ERROR: Could not set pwm args\n");
				exit(-1);
			}
			printf("frequency = %.2f || divisor = %d\n", frequency, divisor);
		}
	}

	pwm0.duty_cycle = 0;
	if(ioctl(fd, SET_PWM_DUTY, &pwm0) != 0)
	{
		printf("ERROR: Could not set pwm args\n");
		exit(-1);
	}
	close(fd);
	
	return 0;
}