# Linux 动态模块与设备驱动  

## Linux 的动态模块（修改系统调用）


<modify_syscall.c>  

中代码可见内核模块组织

```c
module_init(mymodule_init); //注册初始化函数
module_exit(mymodule_exit); //注册清除函数
MODULE_LICENSE("GPL"); //模块许可声明
```

内核模块正是借助这样的组织完成加/卸载的。

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

将上述问题解决后，模块可以正常加载，系统调用被修改。

## Linux 设备驱动（编写字符设备驱动程序）


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



可用以下命令查看使用模块情况  

```c
sudo lsof /dev/chardev0
```

可用以下命令杀死所有使用该模块的进程  

```c
sudo fuser -k /dev/chardev0
```
