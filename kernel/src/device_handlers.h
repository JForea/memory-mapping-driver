#ifndef DEVICE_HANDLERS_H
#define DEVICE_HANDLERS_H

#include <linux/fs.h>
#include <linux/uaccess.h>

int mmd_open(struct inode *inodep, struct file *filep);
int mmd_release(struct inode *inodep, struct file *filep);
long mmd_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

#endif