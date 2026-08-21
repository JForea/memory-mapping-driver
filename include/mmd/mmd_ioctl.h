#ifndef MMD_ARGS_H
#define MMD_ARGS_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define MMD_MAGIC 37745

static const char *DEVICE_NAME = "mmd";

typedef struct {
    __u64 addr;
    __u32 len;
} mmd_args;

#define MMD_ALLOC _IOW(MMD_MAGIC, 0, mmd_args)
#define MMD_FREE _IOW(MMD_MAGIC, 1, mmd_args)

#endif