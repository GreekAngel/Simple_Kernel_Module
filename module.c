#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nakopoulos Stylianos");
MODULE_DESCRIPTION("Simple Kernel Module");
MODULE_VERSION("1.0");

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "module"
#define BUF_LEN 255


static int Major;
static int Device_Open = 0;
static char msg[BUF_LEN];
static char *msg_Ptr;

static struct file_operations fops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release
};

static dev_t devNo;
static struct class *pClass;

int init_module(void)
{
    struct device *pDev;
    printk(KERN_INFO "Loading module...\n");
    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
        printk(KERN_ALERT "Module load failed...\n"
                          "--> Could not register device: %d\n", Major);
        return Major;
    }

    devNo = MKDEV(Major, 0);

    pClass = class_create(THIS_MODULE, "module");
    if (IS_ERR(pClass)) {
        printk(KERN_WARNING "\nCan not create class");
        unregister_chrdev_region(devNo, 1);
        return -1;
    }

    if (IS_ERR(pDev = device_create(pClass, NULL, devNo, NULL, "module"))) {
        printk(KERN_WARNING "abuse.ko can not create device /dev/module\n");
        class_destroy(pClass);
        unregister_chrdev_region(devNo, 1);
        return -1;
    }

    printk(KERN_INFO "Module has been loaded: %d\n", Major);

    return SUCCESS;
}

void cleanup_module(void)
{
    printk(KERN_ALERT "Unloading module...\n");
    device_destroy(pClass, devNo);
    class_destroy(pClass);
    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_ALERT "Module unloaded!\n");
}

static int device_open(struct inode *inode, struct file *file)
{
    static int counter = 1;

    if (Device_Open)
        return -EBUSY;

    Device_Open++;
    sprintf(msg, "\nAuthor: Nakopoulos Stylianos\n"
                 "Project: Simple Kernel Module\n"
                 "Used: %d\n\n", counter++);
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;
    module_put(THIS_MODULE);

    return 0;
}

static ssize_t device_read(struct file *filp,
                           char *buffer,
                           size_t length,
                           loff_t * offset)
{
    int bytes_read = 0;

    if (*msg_Ptr == 0)
        return 0;

    while (length && *msg_Ptr) {

        put_user(*(msg_Ptr++), buffer++);

        length--;
        bytes_read++;
    }

    return bytes_read;
}

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
    return -EINVAL;
}

