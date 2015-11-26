
ifeq ($(KERNELRELEASE),)

KVERSION ?= $(shell uname -r)
BUILD_DIR ?= /lib/modules/${KVERSION}/build

PWD := $(shell pwd)

modules:
	$(MAKE) -C $(BUILD_DIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(BUILD_DIR) M=$(PWD) modules_install

clean:
	rm -rf *~ *.o .*.cmd *.mod.c *.ko *.ko.unsigned .depend \
    	.tmp_versions modules.order Module.symvers Module.markers

.PHONY: modules modules_install clean

else

obj-m := guncon3.o

endif
