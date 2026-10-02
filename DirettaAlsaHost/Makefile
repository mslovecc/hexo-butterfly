MODULENAME = alsa_bridge

ifneq ($(KERNELRELEASE),)
	obj-m := $(MODULENAME).o
else

PWD := $(shell pwd)

default: 


	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif



clean :
	rm -f ./.$(MODULENAME)*
	rm -f ./$(MODULENAME).mod*
	rm -f ./$(MODULENAME).o
	rm -f ./$(MODULENAME).ko
	rm -f -r ./.tmp_versions
	rm -f -r ./Module.symvers
	rm -f -r ./modules.order

# make KERNELDIR=xxxxxxxxxx
