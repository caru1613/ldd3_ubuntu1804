
ifneq ($(KERNELRELEASE),)
	obj-m := scull.o
	module-objs := scull.o
	val=1
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	val=2

default:
	echo $(val)
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc scull_test.c -o scull_test

clean:
	rm -rf *.o *.ko *.mod.* *.symvers modules.order *.cmd .tmp_versions
	rm scull_test

endif
