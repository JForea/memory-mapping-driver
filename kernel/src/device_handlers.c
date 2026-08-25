#include "device_handlers.h"
#include "memory_helpers.h"

#include <linux/errno.h>
#include <mmd/mmd_ioctl.h>

static int parse_args(mmd_args *dst, unsigned long arg) {
	if (copy_from_user(
		dst,
		(const void __user *)arg,
		sizeof(*dst)
	) != 0) {
		return -EFAULT;
	}

	return 0;
}

int mmd_open(struct inode *inodep, struct file *filep) { 
	printk(KERN_INFO "MMD: Device opened.\n");
	return 0; 
}

long mmd_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) { 
	mmd_args args = {};
	int err = 0;

	printk(KERN_INFO "MMD: IOCTL command received - CMD: %u ARG: %lu.\n", cmd, arg);

    switch (cmd) {
    case MMD_ALLOC: {
		err = parse_args(&args, arg);
		if (err < 0) {
			return err;
		}

		err = mem_allocate(args.addr);
		if (err < 0) {
			return err;
		}

		printk(KERN_INFO "MMD: MMD_ALLOC executed succesfully.\n");

		return 0;
	}

	case MMD_FREE: {
		err = parse_args(&args, arg);
		if (err < 0) {
			return err;
		}

		err = mem_free(args.addr);
		if (err < 0) {
			return err;
		}

		printk(KERN_INFO "MMD: MMD_FREE executed succesfully.\n");

		return 0;
	}

    default: {
		return -ENOTTY;
	}
    }
}

int mmd_release(struct inode *inodep, struct file *filep) { 
	printk(KERN_INFO "MMD: Device closed.\n"); 
	return 0; 
}