/*
3.2.1实验目的
(1)学习内核通过动态模块来动态加载内核新功能的机制；
(2)理解LINUX字符设备驱动程序的基本原理；
(3)掌握字符设备的驱动运作机制；
(4)学会编写字符设备驱动程序；
(5)学会编写用户程序通过对字符设备的读写完成不同用户间的通信。
3.2.2实验内容
(1)编译、安装与卸载动态模块；
(2)实现系统调用的篡改；
(3)编写一个简单的字符设备驱动程序，以内核空间模拟字符设备，完成对该设备的打开、读写和释放操作；
(4)编写聊天程序实现不同用户通过该设备实现一对一、一对多、多对多的聊天。
3.2.3实验报告内容要求
报告中要说明驱动程序的实现；用户程序的实现；如何实现读写的同步与互斥；一对一、一对多、多对多的聊天分别是如何实现的。
*/

#include <linux/init.h> /*必须要包含的头文件*/
#include <linux/kernel.h>
#include <linux/module.h> /*必须要包含的头文件*/

static int mymodule_init(void) //模块初始化函数
{
printk("hello,my module was loaded! \n"); /*输出信息到内核日志*/
return 0;
}

static void mymodule_exit(void) //模块清理函数
{
printk("goodbye,unloading my module.\n"); /*输出信息到内核日志*/
}

module_init(mymodule_init); //注册初始化函数
module_exit(mymodule_exit); //注册清理函数
MODULE_LICENSE("GPL v2"); //模块许可声明