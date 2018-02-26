#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
// #include <mach/platform.h>

// https://pinout.xyz/pinout/wiringpi
#define LEDPIN 29		// Physical pin 40 | BCM pin 21 | Wiring Pi pin 29
#define SWITCHPIN 25	// Physical pin 37 | BCM pin 26 | Wiring Pi pin 25

int main(void) 
{
    if(wiringPiSetup() == -1) 
    {
        printf("setup wiringPi failed !\n");
        return -1;
    }

    pinMode(LEDPIN, OUTPUT);
    pinMode(SWITCHPIN, INPUT);
    while(1) 
    {
	#if 0
        digitalWrite(LEDPIN, HIGH);	// LED ON
        printf("led ON\n");
        delay(5000);
        digitalWrite(LEDPIN, LOW);	// LED OFF
        printf("led OFF\n");
        delay(5000);
	#else
        if (digitalRead(SWITCHPIN)) 
        {
            digitalWrite(LEDPIN, HIGH);
        } 
        else 
        {
        	delay(100);
        	digitalWrite(LEDPIN, LOW);
        }
    #endif
    }
    return 0;
}
