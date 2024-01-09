# Linux 动态模块与设备驱动  

## Linux 的动态模块（修改系统调用）

```c
<modify_syscall.c>

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
    cr0 &= 0xfffeffff; //set 0 to the low 17th bit
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
    printk("No 96 syscall has changed to hello");
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
```  

上述代码可见内核模块组织

```c
module_init(mymodule_init); //注册初始化函数
module_exit(mymodule_exit); //注册清除函数
MODULE_LICENSE("GPL"); //模块许可声明
```

内核模块正是借助这样的模块完成加/卸载的。

在这里加载命令为

```c
insmod modify_syscall.ko
```

卸载命令为

```c
rmmod modify_syscall
```

在上面的代码中，需要注意这样的几个点  
* x86 架构中CR0 寄存器的第16位是写保护（Write Protect）标志位。通过将该位设    
为0，代码实现了取消写保护的目的。取消写保护允许修改只读页面的内容，这在一些需要动
态修改页表等情况下是必要的。在执行修改后，应该将CR0恢复到修改前的状态，以维护系统的一致性和安全性。 
* p_sys_call_table是不定的，可以查表，但这过于繁琐且代码不易迁移，因此使用函数
kallsyms_lookup_name()查询。
*  64位机器上系统调用gettimeofday()的系统调用号为96。

将上述问题解决后，模块可以正常加载，系统调用被修改。用modify_old_syscall和modify_new_syscall进行测试，结果如下图。  
加载模块  
![Alt text](img/img1.png)  
卸载模块  
![Alt text](img/img2.png)  

## Linux 设备驱动（编写字符设备驱动程序）

```c
<globalvar.c>

#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<asm/uaccess.h>
#include<linux/wait.h>
#include<linux/semaphore.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include<linux/kdev_t.h>
#include <linux/device.h>

#define MAXNUM 100
#define MAJOR_NUM 456 //主设备号 ，没有被使用
struct Scull_Dev{
    struct cdev devm;        // 字符设备
    struct semaphore sem;    // 信号量，实现读写时的 PV 操作
    wait_queue_head_t outq;  // 等待队列，实现阻塞操作
    int flag;                // 阻塞唤醒标志
    char buffer[MAXNUM + 1]; // 字符缓冲区
    char *rd, *wr, *end;     // 读，写，尾指针
};
struct Scull_Dev globalvar;
static struct class *my_class;
int major = MAJOR_NUM;
static ssize_t globalvar_read(struct file *,char *,size_t ,loff_t *);
static ssize_t globalvar_write(struct file *,const char *,size_t ,loff_t *);
static int globalvar_open(struct inode *inode,struct file *filp);
static int globalvar_release(struct inode *inode,struct file *filp);

struct file_operations globalvar_fops =
{
    .read = globalvar_read,
    .write = globalvar_write,
    .open = globalvar_open,
    .release = globalvar_release,
};
static int globalvar_init(void)
{
    int result = 0;
    int err = 0;
    dev_t dev = MKDEV(major, 0);
    if (major)
    {
        // 静态申请设备编号
        result = register_chrdev_region(dev, 1, "charmem");
    }
    else
    {
        //动态分配设备号
        result = alloc_chrdev_region(&dev, 0, 1, "charmem");
        major = MAJOR(dev);
    }
    if(result < 0)
        return result;
    cdev_init(&globalvar.devm, &globalvar_fops); // 初始化字符设备结构体 globalvar.devm，其中 globalvar_fops 包含了字符设备的操作函数
    globalvar.devm.owner = THIS_MODULE;
    err = cdev_add(&globalvar.devm, dev, 1); // 用 cdev_add 将字符设备添加到系统，这样内核就知道了这个字符设备的存在
    if (err)
        printk(KERN_INFO "Error %d adding char_mem device", err);
    else{
        printk("globalvar register success\n"); 
        sema_init(&globalvar.sem, 1);              // 初始化信号量
        init_waitqueue_head(&globalvar.outq);      // 初始化等待队列
        globalvar.rd = globalvar.buffer;           // 读指针
        globalvar.wr = globalvar.buffer;           // 写指针
        globalvar.end = globalvar.buffer + MAXNUM; // 缓冲区尾指针
        globalvar.flag = 0;                        // 阻塞唤醒标志置 0
    }
    my_class = class_create(THIS_MODULE, "chardev0");  // 使用 class_create 创建一个设备类。
    device_create(my_class, NULL, dev, NULL, "chardev0"); // 使用 device_create 在 /dev 目录下创建一个设备文件，并将其与字符设备关联
    return 0;
}

static int globalvar_open(struct inode *inode,struct file *filp)
{
    try_module_get(THIS_MODULE); // 模块计数加一
    printk("This chrdev is in open\n");
    return (0);
}

static int globalvar_release(struct inode *inode,struct file *filp)
{
    module_put(THIS_MODULE); // 模块计数减一
    printk("This chrdev is in release\n");
    return (0);
}

static void globalvar_exit(void)
{
    device_destroy(my_class, MKDEV(major, 0));
    class_destroy(my_class);
    cdev_del(&globalvar.devm);
    unregister_chrdev_region(MKDEV(major, 0), 1); // 注销设备
}

static ssize_t globalvar_read(struct file *filp,char *buf,size_t len,loff_t *off)
{
    if(wait_event_interruptible(globalvar.outq,globalvar.flag!=0)) //不可读时 阻塞读进程
    {
        return -ERESTARTSYS;
    }
    if(down_interruptible(&globalvar.sem)) //P 操作
    {
        return -ERESTARTSYS;
    }
    globalvar.flag = 0;
    printk("into the read function\n");
    printk("the rd is %c\n", *globalvar.rd); // 读指针
    if (globalvar.rd < globalvar.wr)
        len = min(len, (size_t)(globalvar.wr - globalvar.rd)); // 更新读写长度
    else
        len = min(len, (size_t)(globalvar.end - globalvar.rd));
    printk("the len is %d\n", len);
    if (copy_to_user(buf, globalvar.rd, len))
    {
        printk(KERN_ALERT "copy failed\n");
        up(&globalvar.sem);
        return -EFAULT;
    }
    printk("the read buffer is %s\n",globalvar.buffer);
    globalvar.rd = globalvar.rd + len;
    if(globalvar.rd == globalvar.end)
        globalvar.rd = globalvar.buffer; //字符缓冲区循环
    up(&globalvar.sem); //V 操作
    return len;
}

static ssize_t globalvar_write(struct file *filp,const char *buf,size_t len,loff_t *off)
{
    if(down_interruptible(&globalvar.sem)) //P 操作
    {
        return -ERESTARTSYS;
    }
    if(globalvar.rd <= globalvar.wr)
        len = min(len,(size_t)(globalvar.end - globalvar.wr));
    else
        len = min(len,(size_t)(globalvar.rd-globalvar.wr-1));
    printk("the write len is %d\n",len);
    if(copy_from_user(globalvar.wr,buf,len))
    {
        up(&globalvar.sem); //V 操作
        return -EFAULT;
    }
    printk("the write buffer is %s\n", globalvar.buffer);
    printk("the len of buffer is %d\n", strlen(globalvar.buffer));
    globalvar.wr = globalvar.wr + len;
    if (globalvar.wr == globalvar.end)
        globalvar.wr = globalvar.buffer;    // 循环
    up(&globalvar.sem);                     // V 操作
    globalvar.flag = 1;                     // 条件成立，可以唤醒读进程
    wake_up_interruptible(&globalvar.outq); // 唤醒读进程
    return len;
}

module_init(globalvar_init);
module_exit(globalvar_exit);
MODULE_LICENSE("GPL");
```  

设备文件的 VFS中，每个文件都有一个 inode 与其对应，内核的 inode 结构中的 i_fop
成员，类型为 file_operations,file_operations 定义了文件的各种操作,用户对文件操作是通过调用 file_operations 来实现的，或者说内核使用file_operations 来访问设备驱动程序中的函数,为了使用户对设备文件的操作
能够转换成对设备的操作， VFS 必须在设备文件打开时，改变其 inode 结构中i_fop 成员的默认值，将其替换为与设备相关的具体函数操作。

在这里我们使用的是

```c
struct file_operations globalvar_fops = {
    .read = globalvar_read,
    .write = globalvar_write,
    .open = globalvar_open,
    .release = globalvar_release,
};
```

将该文件的读写打开释放操作变成自定义的，从而完成了字符设备驱动程序的编写。
由于涉及到文件的读写，则必须引入互斥机制。

利用flag和PV操作实现了写读相关的互斥。 
这里使用down/up_interruptible函数，它们是是 Linux 内核中用于进行 PV 操作的函数，是信号可中断的版本。
对于字符设备等场景，通常使用这个函数来进行信号量的 PV 操作。使用其他的 PV 操作函数，可以考虑down/up()函数，其信号不可中断，不响应中断信号

测试见下图。  

![Alt text](img/img3.png)  
![Alt text](img/img4.png)  

可用以下命令查看使用模块情况  

```c
sudo lsof /dev/chardev0
```

可用以下命令杀死所有使用该模块的进程  

```c
sudo fuser -k /dev/chardev0
```
