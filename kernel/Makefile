PROJECT_NAME := cmmap

SRC_DIR ?= src

ifneq ($(KERNELRELEASE),)

obj-m += $(PROJECT_NAME).o
$(PROJECT_NAME)-y := $(SRC_DIR)/main.o $(SRC_DIR)/device_handlers.o $(SRC_DIR)/memory_helpers.o
ccflags-y += -I$(src)/../include

else

KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR := $(CURDIR)/build

CC ?= gcc
CFLAGS := -Wall -Wextra -Werror -Isrc
LDLIBS := -lcunit

USER_TEST_BIN := $(BUILD_DIR)/user
USER_TEST_SRC := tests/user/helpers-tests.c

.PHONY: module test load unload clean

module:
	mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) MO=$(BUILD_DIR) modules

load: module
	sudo insmod $(BUILD_DIR)/$(PROJECT_NAME).ko

unload:
	sudo rmmod $(PROJECT_NAME)

clean:
	rm -rf $(BUILD_DIR)

endif