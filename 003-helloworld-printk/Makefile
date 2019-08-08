obj-m += helloworld.o

all:
	@$(MAKE) -C $(KERNEL_ROOT) M=$(shell pwd) modules
clean:
	@$(MAKE) -C $(KERNEL_ROOT) M=$(shell pwd) clean
