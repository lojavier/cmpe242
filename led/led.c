#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include "led.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lorenzo Javier");
MODULE_DESCRIPTION("GPIO LED Driver");
MODULE_VERSION("0.1");

#define DEVICE_NAME     "led0"
#define CLASS_NAME      "led"
 
static unsigned int  majorNumber;
static unsigned int  numberOpens = 0;
static struct class*  ledClass  = NULL;
static struct device* ledDevice = NULL;
static unsigned int  led_level_0 = 0;
static unsigned int irqNumber;
static unsigned int numberPresses = 0;

static int      dev_open(struct inode *, struct file *);
static int      dev_release(struct inode *, struct file *);
static ssize_t  dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t  dev_write(struct file *, const char *, size_t, loff_t *);
static long     dev_ioctl(struct file *, unsigned int, unsigned long);
static irq_handler_t  led_irq_handler(unsigned int , void *, struct pt_regs *);

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl
};

static int __init led_init(void)
{
    int result = 0;
    printk(KERN_INFO "LED: Initializing...\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "LED failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "LED: registered correctly with major number %d\n", majorNumber);

    ledClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(ledClass))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(ledClass);
    }
    printk(KERN_INFO "LED: device class registered correctly\n");

    ledDevice = device_create(ledClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(ledDevice))
    {
        class_destroy(ledClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(ledDevice);
    }

    gpio_request(GPIO_LED_0, "sysfs");
    gpio_direction_output(GPIO_LED_0, led_level_0);
    gpio_export(GPIO_LED_0, false);
    gpio_request(GPIO_BUTTON_0, "sysfs");
    gpio_direction_input(GPIO_BUTTON_0);
    gpio_set_debounce(GPIO_BUTTON_0, 200);
    gpio_export(GPIO_BUTTON_0, false);
    printk(KERN_INFO "LED: The button state is currently: %d\n", gpio_get_value(GPIO_BUTTON_0));
    irqNumber = gpio_to_irq(GPIO_BUTTON_0);
    printk(KERN_INFO "LED: The button is mapped to IRQ: %d\n", irqNumber);
    result = request_irq(irqNumber,
                        (irq_handler_t) led_irq_handler,
                        IRQF_TRIGGER_RISING,
                        "led_gpio_handler",
                        NULL);

    printk(KERN_INFO "LED: Initializing done!\n");
    return 0;
}

static void __exit led_exit(void)
{
    printk(KERN_INFO "LED: Exiting...\n");

    gpio_set_value(GPIO_LED_0, 0);
    gpio_unexport(GPIO_LED_0);
    free_irq(irqNumber, NULL);
    gpio_unexport(GPIO_BUTTON_0);
    gpio_free(GPIO_LED_0);
    gpio_free(GPIO_BUTTON_0);

    device_destroy(ledClass, MKDEV(majorNumber, 0));
    class_unregister(ledClass);
    class_destroy(ledClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);

    printk(KERN_INFO "LED: Exiting done!\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    numberOpens++;
    printk(KERN_INFO "LED: Device has been opened %d time(s)\n", numberOpens);
    return 0;
}

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
    printk(KERN_INFO "LED: Setting led level to %d...\n", led_level_0);
    if (kstrtoint(buffer, 10, &led_level_0) == 0)
    {
        gpio_set_value(GPIO_LED_0, led_level_0);
    }
    printk(KERN_INFO "LED: Setting led level done!\n");
    return 0;
}

static long dev_ioctl(struct file *filep, unsigned int ioctl_level, unsigned long ioctl_gpio)
{
    printk(KERN_INFO "LED: dev_ioctl | ioctl_level = %d\n", ioctl_level);

    switch (ioctl_level)
    {
        case GPIO_LED_OFF:
            gpio_set_value(ioctl_gpio, GPIO_LED_OFF);
            break;

        case GPIO_LED_ON:
            gpio_set_value(ioctl_gpio, GPIO_LED_ON);
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "LED: Device successfully closed\n");
    return 0;
}

static irq_handler_t led_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
   led_level_0 = !led_level_0;
   gpio_set_value(GPIO_LED_0, led_level_0);
   printk(KERN_INFO "LED: Interrupt! (button state is %d)\n", gpio_get_value(GPIO_BUTTON_0));
   numberPresses++;
   return (irq_handler_t) IRQ_HANDLED;
}

module_init(led_init);
module_exit(led_exit);