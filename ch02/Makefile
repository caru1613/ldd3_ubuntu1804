
ifneq ($(KERNELRELEASE),)
	obj-m := hello.o
	module-objs := hello.o
	val=1
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	val=2

default:
	echo $(val)
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers modules.order *.cmd .tmp_versions

endif
