#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/kprobes.h>

//original,syscall 96 function: gettimeofday
// new syscall 96 function: print "No 96 syscall has changed to hello"and return a+b

#define sys_No 96
unsigned long old_sys_call_func;
unsigned long p_sys_call_table; // find in /boot/System.map-'uname -r'
unsigned long orig_cr0;

unsigned int clear_cr0(void)        
{
        unsigned int cr0 = 0;
        unsigned int ret;
        //move the value in reg cr0 to reg rax
        //movl moves a 32-bits operand
        //movq moves a 64-bits operand
        //rax is a 64-bits register
        //an assembly language code
        //asm volatile ("movl %%cr0, %%eax" : "=a"(cr0));//32-bits        
        asm volatile ("movq %%cr0, %%rax" : "=a"(cr0));        //64-bits
        ret = cr0;
        //var cr0 is rax        
        cr0 &= 0xfffeffff; //set 0 to the 17th bit
        //asm volatile ("movl %%eax, %%cr0" :: "a"(cr0));//32-bits
        //note that cr0 above is a variable while cr0 below is a reg.        
        asm volatile ("movq %%rax, %%cr0" :: "a"(cr0));        
        return ret;
}

//recover the value of WP 
void setback_cr0(unsigned int val)
{        
        //asm volatile ("movl %%eax, %%cr0" :: "a"(val));//32-bits
        asm volatile ("movq %%rax, %%cr0" :: "a"(val));//64-bits
}

asmlinkage int hello(int a,int b) //new function
{
    printk("No 78 syscall has changed to hello");
    return a + b;
}

void modify_syscall(void)
{
	unsigned long *sys_call_addr;
	p_sys_call_table = (unsigned long )kallsyms_lookup_name("sys_call_table");
	if (!p_sys_call_table) {
        pr_err("Failed to find sys_call_table address\n");
        return;
    	}
        
    sys_call_addr = (unsigned long *)(p_sys_call_table + sys_No * sizeof(void *));
    old_sys_call_func = *(sys_call_addr);
	orig_cr0 = clear_cr0(); 
    *(sys_call_addr) = (unsigned long)&hello; // point to new function
    setback_cr0(orig_cr0);
}

void restore_syscall(void)
{
    unsigned long *sys_call_addr;

    sys_call_addr = (unsigned long *)(p_sys_call_table + sys_No * sizeof(void *));
	orig_cr0 = clear_cr0();
    *(sys_call_addr) = old_sys_call_func; // point to original function
setback_cr0(orig_cr0);
}

static int mymodule_init(void)
{
    modify_syscall();
    return 0;
}

static void mymodule_exit(void)
{
    restore_syscall();
}

module_init(mymodule_init);
module_exit(mymodule_exit);
MODULE_LICENSE("GPL");