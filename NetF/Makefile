# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq (${KERNELRELEASE},)
	obj-m += NetF.o
# Otherwise we were called directly from the command line.
# Invoke the kernel build system.
else
KERNEL_SOURCE = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
all:
	make -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
clean:
	make -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif
