#ifndef ADC_H
#define ADC_H

#define GPIO_PWM_CCW    	0x1	// Counterclockwise Rotation
#define GPIO_PWM_CW     	0x0	// Clockwise Rotation
// #define GPIO_PWM_PIN		18	// BCM pin 18
#define GPIO_PWM_ROTATE_PIN	23	// BCM pin 23

// #define STEPS_PER_REV 	1600 // Steps per Revolution (360 degrees / 0.225)

#define DEVICE_NAME         "pwm"
#define CLASS_NAME          "pwm-lorenzo"

#define HIGH    0x1
#define LOW     0x0
#define OUTPUT  0x1
#define INPUT   0x0

static unsigned int PWMMODE = 0;
static unsigned int MSMODE = 1;
//Two PWM modes
#define ERRFREQ 1
#define ERRCOUNT 2
#define ERRDUTY 3
#define ERRMODE 4
//Error Codes

//Private constants
#define BCM2835_PERI_BASE 0x20000000
#define PWM_BASE (BCM2835_PERI_BASE + 0x20C000) /* PWM controller */
#define CLOCK_BASE (BCM2835_PERI_BASE + 0x101000) /* Clock controller */
#define GPIO_BASE (BCM2835_PERI_BASE + 0x200000) /* GPIO controller */
//Base register addresses
#define PWM_CTL 0
#define PWM_RNG1 4
#define PWM_DAT1 5
#define PWMCLK_CNTL 40
#define PWMCLK_DIV 41
// Register addresses offsets divided by 4 (register addresses are word (32-bit) aligned 
#define GPIO_BLOCK_SIZE 4096
// Block size.....every time mmap() is called a 4KB 
//section of real physical memory is mapped into the memory of
//the process

static unsigned int counts = 256; // PWM resolution

enum PWM_FUNC
{
	ENABLE_PWM,
	SET_PWM_ROT,
	SET_PWM_FREQ,
	SET_PWM_DUTY,
	MODIFY_PWM_DUTY
};

#endif