#include <linux/init.h>
#include <linux/module.h>
#include <linux/device/class.h>
#include <linux/device.h>

#include <mmd/mmd_ioctl.h>

#include "device_handlers.h"

static dev_t devt;

static struct class *cls;

static int major;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JForea");
MODULE_DESCRIPTION("Linux kernel module for adding custom memory mapping function.");

static struct file_operations fops = {
    .open = mmd_open,
    .unlocked_ioctl = mmd_ioctl,
    .release = mmd_release,
};

static char *fad_devnode(const struct device *dev, umode_t *mode) {
    /*
    * Only root user can access device.
    */
    if (mode)
        *mode = 0600;

    return NULL;
}

static int __init mmd_init(void) {
    struct device *dev;

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "MMD: Failed to register major number.\n"); 
        return major;
    }
    devt = MKDEV(major, 0);

    cls = class_create(DEVICE_NAME);
    if (IS_ERR(cls)) {
        printk(KERN_ALERT "MMD: Failed to create device class.\n");
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(cls);
    }
    cls->devnode = fad_devnode;

    dev = device_create(cls, NULL, devt, NULL, DEVICE_NAME);
    if (IS_ERR(dev)) {
        printk(KERN_ALERT "MMD: Failed to create device.\n");
        class_destroy(cls);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(dev);
    }

    printk(KERN_INFO "MMD: File access driver started. Device character registered with major %d\n", major);
    return 0;
}

static void __exit mmd_exit(void) {
    device_destroy(cls, devt);
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "MMD: File access driver stopped.\n");
}

module_init(mmd_init);

module_exit(mmd_exit);