obj-m += test.o #need to match to file name
 
PWD := $(CURDIR) 
 
all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod test.ko	
 
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	sudo rmmod test
