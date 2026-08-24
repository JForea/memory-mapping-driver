#ifndef MMD_H
#define MMD_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <mmd_ioctl.h>
#include <cerrno>

void __get_device(char *dev) {
    strcpy(dev, "/dev/");
    strcat(dev, DEVICE_NAME);
}

int __open_device(void) {
    int fd;
    char device[20];

    __get_device(device);

    fd = open(device, O_RDWR);
    if (fd < 0) {
        if (errno == EACCES)
            return -EACCES;
        return -EFAULT;
    }

    return fd;
}

int mem_allocate(unsigned long addr, unsigned int len) {
    int fd;
    mmd_args args;

    fd = __open_device();

    if (fd < 0) {
        return fd;
    }

    args = {
        .addr = addr,
        .len = len,
    };

    if (ioctl(fd, MMD_ALLOC, &args)) {
        close(fd);
        return -EFAULT;
    }

    close(fd);
}

int mem_free(unsigned long addr, unsigned int len) {
    int fd;
    mmd_args args;

    fd = __open_device();

    if (fd < 0) {
        return fd;
    }

    args = {
        .addr = addr,
        .len = len,
    };

    if (ioctl(fd, MMD_FREE, &args)) {
        close(fd);
        return -EFAULT;
    }

    close(fd);
}

#endif