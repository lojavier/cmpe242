#include <stdio.h>
#include <time.h>
#include <iostream>
#include <wiringPi.h>
// #include <softPwm.h>

using namespace std;

#define DIR				4	// DIR  Physical pin 16 | BCM pin 23 | Wiring Pi pin 4
#define STEP 			1	// STEP Physical pin 12 | BCM pin 18 | Wiring Pi pin 1
#define MS1 			5	// MS1 	Physical pin 18 | BCM pin 24 | Wiring Pi pin 5
#define MS2 			6	// MS2 	Physical pin 22 | BCM pin 25 | Wiring Pi pin 6
#define CW 				1   // Clockwise Rotation
#define CCW 			0	// Counterclockwise Rotation
#define CPU_SPEED		700000000	// 700 MHz

// #define ServoMotorPin 29	// Physical pin 40 | BCM pin 21 | Wiring Pi pin 29
// #define ServoMotorPin 1		// Physical pin 12 | BCM pin 18 | Wiring Pi pin 1

int main(int argc, char** argv)
{
	cout << argc << endl;
	for(int i = 0; i < argc; ++i)
	{
		cout << argv[i] << endl;
	}
	if(wiringPiSetup()==-1)
	{
		cout << "Setup wiringPi failed!" << endl;
		return -1;
	}

	FILE * pFile;
	pFile = fopen ("myfile.txt","w");
	if (pFile!=NULL)
	{
		fputs ("fopen example",pFile);
		fclose (pFile);
	}

	enum MICROSTEP
	{
		FULL_STEP,
		HALF_STEP,
		QUARTER_STEP,
		EIGHTH_STEP
	};


	uint32_t steps_per_revolution = 1600;
	uint32_t microstep_resolution = atoi(argv[1]);
	uint32_t duty_cycle = atoi(argv[2]);
	uint32_t delay_period = atoi(argv[2]);
	int ms1_level = LOW;
	int ms2_level = LOW;
	
	pinMode(DIR, OUTPUT);
	pinMode(STEP, OUTPUT);
	pinMode(MS1, OUTPUT);
	pinMode(MS2, OUTPUT);

	switch(microstep_resolution)
	{
		case FULL_STEP:
			ms1_level = LOW;
			ms2_level = LOW;
			steps_per_revolution = steps_per_revolution / 8;
			break;

		case HALF_STEP:
			ms1_level = HIGH;
			ms2_level = LOW;
			steps_per_revolution = steps_per_revolution / 4;
			break;

		case QUARTER_STEP:
			ms1_level = LOW;
			ms2_level = HIGH;
			steps_per_revolution = steps_per_revolution / 2;
			break;

		case EIGHTH_STEP:
			ms1_level = HIGH;
			ms2_level = HIGH;
			steps_per_revolution = steps_per_revolution / 1;
			break;

		default:
			break;
	}

	digitalWrite(MS1, ms1_level);
	digitalWrite(MS2, ms2_level);

	digitalWrite(DIR, CW);
	for(uint32_t i = 0; i <= steps_per_revolution; i++)
	{
		digitalWrite(STEP, HIGH);
		delay(delay_period);
		digitalWrite(STEP, LOW);
		delay(delay_period);
	}

	digitalWrite(DIR, CCW);
	for(uint32_t i = 0; i <= steps_per_revolution; i++)
	{
		digitalWrite(STEP, HIGH);
		delay(delay_period);
		digitalWrite(STEP, LOW);
		delay(delay_period);
	}
	
	/*
	softPwmCreate(ServoMotorPin, 0, 100);
	softPwmWrite(ServoMotorPin, 0);

	while(1)
	{
		for(int i = 0; i < 100; i++)
		{
			softPwmWrite(ServoMotorPin, i);
			delay(50);
		}
		for(int i = 100; i > 0; i--)
		{
			softPwmWrite(ServoMotorPin, i);
			delay(10);
		}
	}
	*/

	/*
	float delay_period = 0.01;
	pinMode(ServoMotorPin, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(192);
	pwmSetRange(2000);
	while(1)
	{
		for(int i = 50; i <= 2000; i++)
		{
			pwmWrite(ServoMotorPin, i);
			delay(delay_period);
		}
		delay(1);
		for(int i = 2000; i >= 50; i--)
		{
			pwmWrite(ServoMotorPin, i);
			delay(delay_period);
		}
	}
	*/
	
	return 0;
}