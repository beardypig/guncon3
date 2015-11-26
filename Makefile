TOPDIR          := /usr/src/linux-headers-3.13.0-68-generic
MOD_ROOT        :=
PWD             := $(shell pwd)

obj-m           := guncon3.o

default:
	$(MAKE) -C $(TOPDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f guncon3.o guncon3.ko
	rm -f guncon3.mod.c guncon3.mod.o
	rm -f Module.symvers
	rm -f .guncon3*
	rm -fr .tmp_versions

install:
	$(MAKE) -C $(TOPDIR) SUBDIRS=$(PWD) modules_install
	depmod -a

