
ifneq ($(KERNELRELEASE),)
obj-m := modify_syscall.o mymodules.o globalvar.o # 编译成外部模块
else
KERNELDIR := /lib/modules/$(shell uname -r)/build # 定义一个变量，指向内核目录
PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules # 编译内核模块

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean # 清理编译产生的文件
	rm -f modify_old_syscall modify_new_syscall

all : modify_old_syscall modify_new_syscall read write 
.PHONY : all
modify_old_syscall: modify_old_syscall.c
	gcc -o $@ $<
modify_new_syscall: modify_new_syscall.c
	gcc -o $@ $<
read: read.c
	gcc -o $@ $<
write: write.c
	gcc -o $@ $<

	
endif



