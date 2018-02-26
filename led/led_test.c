#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "led.h"
 
int main(int argc, char **argv)
{
	int fd;
	int input;

	fd = open("/dev/led0", O_RDWR);
	if (fd < 0)
	{
		perror("Failed to open /dev/led0");
		return errno;
	}

	while(1)
	{
		printf("Set LED level ? ON = 1 : OFF = 0\n");
		scanf("%d", &input);
		switch(input)
		{
			case GPIO_LED_OFF:
				if(ioctl(fd, GPIO_LED_OFF, GPIO_LED_0))
				{
					close(fd);
					perror("Failed to set gpio led level");
					return errno;
				}
				break;
			case GPIO_LED_ON:
				if(ioctl(fd, GPIO_LED_ON, GPIO_LED_0))
				{
					close(fd);
					perror("Failed to set gpio led level");
					return errno;
				}
				break;
			default:
				goto exit_loop;
		}
	}

exit_loop:

	close(fd);
	
	return 0;
}