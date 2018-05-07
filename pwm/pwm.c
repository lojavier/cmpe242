#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/pwm.h>
#include <linux/uaccess.h>
#include "pwm.h"

// http://www.hertaville.com/rpipwm.html
// http://www.airspayce.com/mikem/bcm2835/pwm_8c-example.html

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lorenzo Javier");
MODULE_DESCRIPTION("GPIO PWM Driver");
MODULE_VERSION("0.1");

static volatile unsigned long *clk, *pwm, *gpio;
static unsigned int majorNumber;
static struct class*  pwmClass  = NULL;
static struct device* pwmDevice = NULL;

static int      dev_open(struct inode *, struct file *);
static int      dev_release(struct inode *, struct file *);
// static ssize_t  dev_read(struct file *, char *, size_t, loff_t *);
// static ssize_t  dev_write(struct file *, const char *, size_t, loff_t *);
static void     set_rotation(unsigned int);
static void     set_mode(unsigned int);
static void     modify_duty_cycle(unsigned int);
static void     set_duty_cycle(unsigned int);
static void     set_divisor(unsigned int);
static long     dev_ioctl(struct file *, unsigned int, unsigned long);

static struct file_operations fops =
{
    .open = dev_open,
    // .read = dev_read,
    // .write = dev_write,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl
};

static int __init pwm_init(void)
{
    // struct resource *mem;
    printk(KERN_INFO "PWM: Initializing...\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "PWM failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "PWM: registered correctly with major number %d\n", majorNumber);

    pwmClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(pwmClass))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(pwmClass);
    }
    printk(KERN_INFO "PWM: device class registered correctly\n");

    pwmDevice = device_create(pwmClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(pwmDevice))
    {
        class_destroy(pwmClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(pwmDevice);
    }

    // mem = request_mem_region(CLOCK_BASE, GPIO_BLOCK_SIZE, "clk_base");
    clk = ioremap(CLOCK_BASE, GPIO_BLOCK_SIZE);
    if ((clk = ioremap(CLOCK_BASE, GPIO_BLOCK_SIZE)) == NULL)
    {
        printk(KERN_ERR "Mapping CLOCK failed\n");
        return -1;
    }

    // mem = request_mem_region(GPIO_BASE, GPIO_BLOCK_SIZE, "gpio_base");
    gpio = ioremap(GPIO_BASE, GPIO_BLOCK_SIZE);
    if ((gpio = ioremap(GPIO_BASE, GPIO_BLOCK_SIZE)) == NULL)
    {
        printk(KERN_ERR "Mapping GPIO failed\n");
        return -1;
    }

    // mem = request_mem_region(PWM_BASE, GPIO_BLOCK_SIZE, "pwm_base");
    pwm = ioremap(PWM_BASE, GPIO_BLOCK_SIZE);
    if ((pwm = ioremap(PWM_BASE, GPIO_BLOCK_SIZE)) == NULL)
    {
        printk(KERN_ERR "Mapping CLOCK failed\n");
        return -1;
    }

    // Enable GPIO 18 -> ALT5 -> PWM0
    *(gpio+1) &= ~(7 << 24);
    *(gpio+1) |= (2 << 24);

    set_divisor(1);
    set_duty_cycle(0);
    set_mode(PWMMODE);

    gpio_request(GPIO_PWM_ROTATE_PIN, "sysfs");
    gpio_direction_output(GPIO_PWM_ROTATE_PIN, OUTPUT);
    gpio_export(GPIO_PWM_ROTATE_PIN, false);
    set_rotation(GPIO_PWM_CW);

    printk(KERN_INFO "PWM: Initializing done!\n");
    return 0;
}

static void __exit pwm_exit(void)
{
    printk(KERN_INFO "PWM: Exiting...\n");

    // gpio_set_value(GPIO_PWM_PIN, 0);

    // gpio_unexport(GPIO_PWM_PIN);
    // gpio_free(GPIO_PWM_PIN);

    gpio_unexport(GPIO_PWM_ROTATE_PIN);
    gpio_free(GPIO_PWM_ROTATE_PIN);

    //lets put the PWM peripheral registers in their original state   
    *(pwm + PWM_CTL) = 0;
    *(pwm + PWM_RNG1) = 0x20;
    *(pwm + PWM_DAT1) = 0;

    iounmap(pwm);
    // release_mem_region(PWM_BASE, GPIO_BLOCK_SIZE);

    *(clk + PWMCLK_CNTL) = 0x5A000000 | (1 << 5);
    msleep(10);

    // wait until busy flag is set
    while ( (*(clk + PWMCLK_CNTL)) & 0x00000080){}

    //reset divisor
    *(clk + PWMCLK_DIV) = 0x5A000000;
    msleep(10);

    // source=osc and enable clock
    *(clk + PWMCLK_CNTL) = 0x5A000011;

    iounmap(clk);
    // release_mem_region(CLOCK_BASE, GPIO_BLOCK_SIZE);
    
    *(gpio+1) &= ~(7 << 24);

    iounmap(gpio);
    // release_mem_region(GPIO_BASE, GPIO_BLOCK_SIZE);

    device_destroy(pwmClass, MKDEV(majorNumber, 0));
    class_unregister(pwmClass);
    class_destroy(pwmClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);

    printk(KERN_INFO "PWM: Exiting done!\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "PWM: Device has been opened\n");

    *(gpio+1) &= ~(7 << 24);
    *(gpio+1) |= (2<<24);

    return 0;
}

/*
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    unsigned int gpio_pin_num = 0;
    if(kstrtoint(buffer, 10, &gpio_pin_num) == 0) 
    {
        return -EINVAL;
    }
    return gpio_get_value(gpio_pin_num);
}
 
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "PWM: Setting pwm level to %d...\n", pwm_level_0);
    if (kstrtoint(buffer, 10, &pwm_level_0) == 0)
    {
        gpio_set_value(GPIO_PWM_PIN, pwm_level_0);
    }
    printk(KERN_INFO "PWM: Setting pwm level done!\n");
    return 0;
}
*/

static void set_rotation(unsigned int rotation)
{
    gpio_set_value(GPIO_PWM_ROTATE_PIN, rotation);
}

static void set_divisor(unsigned int divisor)
{
    printk(KERN_INFO "PWM: Divisor = %d\n", divisor);

    *(clk + PWMCLK_CNTL) = 0x5A000000 | (1 << 5);
    msleep(10);

    // wait until busy flag is set 
    while ( (*(clk + PWMCLK_CNTL)) & 0x00000080){} 

    //set divisor
    *(clk + PWMCLK_DIV) = 0x5A000000 | (divisor << 12);

    // source=osc and enable clock
    *(clk + PWMCLK_CNTL) = 0x5A000011;

    printk(KERN_INFO "PWM: Frequency set!\n");
}

static void modify_duty_cycle(unsigned int duty_cycle)
{
    printk(KERN_INFO "PWM: Duty cycle = %d %%\n", duty_cycle);

    //set  duty cycle
    *(pwm + PWM_DAT1) = duty_cycle;

    printk(KERN_INFO "PWM: Duty cycle set!\n");
}

static void set_duty_cycle(unsigned int duty_cycle)
{
    printk(KERN_INFO "PWM: Counts = %d\n", counts);
    printk(KERN_INFO "PWM: Duty cycle = %d %%\n", duty_cycle);

    // disable PWM & start from a clean slate
    *(pwm + PWM_CTL) = 0;

    // needs some time until the PWM module gets disabled, without the delay the PWM module crashs
    msleep(10);

    // set the number of counts that constitute a period with 0 for 20 milliseconds = 320 bits
    *(pwm + PWM_RNG1) = counts;
    msleep(10);

    //set  duty cycle
    *(pwm + PWM_DAT1) = duty_cycle;
    msleep(10);

    printk(KERN_INFO "PWM: Duty cycle set!\n");
}

static void set_mode(unsigned int mode)
{
    // start PWM1 in
    if(mode == PWMMODE) //PWM mode 
    {
        *(pwm + PWM_CTL) |= (1 << 0);
        printk(KERN_INFO "PWM: PWM mode enabled\n");
    }
    else // M/S Mode
    {
        *(pwm + PWM_CTL) |= ( (1 << 7) | (1 << 0) );
        printk(KERN_INFO "PWM: M/S mode enabled\n");
    }
}

static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    struct pwm_args
    {
        unsigned int mode;
        unsigned int rotation;
        unsigned int divisor;
        unsigned int duty_cycle;
    };
    struct pwm_args pwm0;
    
    copy_from_user(&pwm0 , (void __user *) arg, sizeof(pwm0));
    printk(KERN_INFO "PWM: mode = %d\n", pwm0.mode);
    printk(KERN_INFO "PWM: rotation = %d\n", pwm0.rotation);
    printk(KERN_INFO "PWM: divisor = %d\n", pwm0.divisor);
    printk(KERN_INFO "PWM: duty_cycle = %d\n", pwm0.duty_cycle);

    switch (cmd)
    {
        case ENABLE_PWM:
            printk(KERN_INFO "PWM: Setting pwm args...\n");
            set_rotation(pwm0.rotation);
            set_divisor(pwm0.divisor);
            set_duty_cycle(pwm0.duty_cycle);
            set_mode(pwm0.mode);
            break;

        case SET_PWM_ROT:
            printk(KERN_INFO "PWM: Setting rotation...\n");
            set_rotation(pwm0.rotation);
            break;

        case SET_PWM_FREQ:
            printk(KERN_INFO "PWM: Setting frequency...\n");
            set_divisor(pwm0.divisor);
            break;

        case SET_PWM_DUTY:
            printk(KERN_INFO "PWM: Setting duty cycle...\n");
            set_duty_cycle(pwm0.duty_cycle);
            break;

        case MODIFY_PWM_DUTY:
            printk(KERN_INFO "PWM: Modifying duty cycle...\n");
            modify_duty_cycle(pwm0.duty_cycle);
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    *(pwm + PWM_CTL) = 0;
    *(pwm + PWM_RNG1) = 0x20;
    *(pwm + PWM_DAT1) = 0;

    *(clk + PWMCLK_CNTL) = 0x5A000000 | (1 << 5);
    msleep(10);

    while ( (*(clk + PWMCLK_CNTL)) & 0x00000080){}

    *(clk + PWMCLK_DIV) = 0x5A000000;
    msleep(10);

    *(clk + PWMCLK_CNTL) = 0x5A000011;

    printk(KERN_INFO "PWM: Device successfully closed\n");
    return 0;
}

module_init(pwm_init);
module_exit(pwm_exit);