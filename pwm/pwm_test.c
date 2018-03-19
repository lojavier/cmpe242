#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "pwm.h"
 
int main(int argc, char **argv)
{
	int fd;
	double period = 0;
	double countDuration;
	double frequency = 1000.0;
	unsigned int divisor = 0;

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
    pwm0.duty_cycle = 0;

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
		else if(strcmp(argv[i],"-f") == 0)
		{
			frequency = (double)atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-d") == 0)
		{
			pwm0.duty_cycle = (unsigned int)((atof(argv[i+1]) / 100.0) * counts);
			i++;
		}
	}

	fd = open("/dev/pwm", O_RDWR);
	if(fd < 0)
	{
		perror("Failed to open /dev/pwm");
		return errno;
	}

	period = 1.0 / frequency;
	countDuration = period / (counts*1.0f);
	divisor = (unsigned int)(19200000.0f / (1.0/countDuration));

	if( divisor < 0 || divisor > 4095 ) 
	{
		printf("divisor value must be between 0-4095\n");
		exit(-1);
	}
	pwm0.divisor = divisor;

	// Initialize values for PWM
	if(ioctl(fd, ENABLE_PWM, &pwm0) != 0)
	{
		printf("ERROR: Could not set pwm args\n");
		exit(-1);
	}

	sleep(5);

#if 1
	for (int i = 104; i >= 14; i-=10)
	{
		pwm0.divisor = i;
		printf("\n\ni = %d\n\n", i);
		if(ioctl(fd, ENABLE_PWM, &pwm0) != 0)
		{
			printf("ERROR: Could not set pwm args\n");
			exit(-1);
		}
		sleep(5);
	}
#else
	for (int i = 0; i < 100; i+=5)
	{
		pwm0.duty_cycle = i;
		printf("\n\ni = %d\n\n", i);
		if(ioctl(fd, MODIFY_PWM_DUTY, &pwm0) != 0)
		{
			printf("ERROR: Could not set pwm args\n");
			exit(-1);
		}
		sleep(5);
	}
#endif

	pwm0.duty_cycle = 0;
	if(ioctl(fd, SET_PWM_DUTY, &pwm0) != 0)
	{
		printf("ERROR: Could not set pwm args\n");
		exit(-1);
	}
	close(fd);
	
	return 0;
}