# lab2

# PART 1

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

int flag = 0; // Whether to be stopped by SIGINT or SIGQUIT
pid_t pid1 = -1, pid2 = -1;

void inter_handler(int signum) {
    if (signum == SIGINT || signum == SIGQUIT || signum == SIGALRM){
        flag = signum;
    }
}

void child_handler(int signum) {
    if (signum == 16 || signum == 17) {
        printf("\n%d stop test\n",signum);
        if (pid2 == -1) {
            printf("\nChild process1 is killed by parent!!\n");
        } else if (pid2 == 0) {
            printf("\nChild process2 is killed by parent!!\n");
        }
        exit(0);
    }
}

void waiting() {
    while(flag == 0){
        ;
    }
}

int main() 
{
    signal(SIGINT, inter_handler);
    signal(SIGQUIT, inter_handler);
    signal(SIGALRM, inter_handler);
    alarm(5);
    while (pid1 == -1)pid1 = fork();
    if (pid1 > 0) {
        while (pid2 == -1)pid2 = fork();
        if (pid2 > 0) {
            // Father
            sleep(0.5);
            waiting();
            printf("\n%d stop test\n",flag);

            kill(pid1, 16);
            kill(pid2, 17);
            
            wait(NULL);
            wait(NULL);

            printf("\nParent process is killed!!\n");
        } 
        else {
            // Child 2
            signal(16, child_handler);
            signal(17, child_handler);
            while(1){
                ;
            }
        }
    } 
    else {
        // Child 1
        signal(16, child_handler);
        signal(17, child_handler);
        while(1){
            ;
        }
    }
    return 0;
}
```

## 1

我最初认为，程序将创建两个子进程，然后父进程会等待接收 SIGINT、SIGQUIT 或等待5秒钟后产生的SIGALRM，接着父进程会向两个子进程发送不同的信号(16 和 17)，子进程将相应地处理这些信号并终止，最终父进程也将终止。

## 2 

![Alt text](img/img1.png)  
图1  
![Alt text](img/img2.png)  
图2  
![Alt text](img/img3.png)  
图3  

* 如果在5秒内父进程接收到 SIGINT 或 SIGQUIT 信号，那么程序会在父进程接收到信号后立即向两个子进程分别发送信号16和17，子进程将相应地终止。最后，父进程终止。

* 如果在5秒内父进程没有接收到 SIGINT 或 SIGQUIT 信号，那么会收到SIGALRM信号，然后父进程将向两个子进程分别发送信号16和17，子进程将相应地终止。最后，父进程终止。

* 差别:父进程接收到的信号不同。

* 程序的结果取决于父进程是否在5秒内接收到中断信号。  

## 3

如果将 SIGALRM 信号发送给父进程，程序将在5秒后终止，而不管是否接收到 SIGINT 或 SIGQUIT 信号。

## 4

kill 命令在程序中使用了两次。是在父进程中，它向 pid1 发送信号16，向 pid2 发送信号17。这两次 kill 命令的目的是向子进程发送信号，以通知它们终止。子进程中存在信号处理程序，当它们接收到这些信号时，将执行 child_handler 函数中的代码，并调用 exit(0) 来终止自身。因此kill之后会打印相应的子程序结束信息。

## 5

* 通过调用 exit(status) 或 _exit(status) 函数，其中 status 是进程的退出状态码。
* 通过接收一个终止信号，例如 SIGTERM，并在信号处理程序中调用 exit() 函数。
* 使用 exit() 函数来主动退出进程通常更可靠，因为它允许进程在退出前进行清理操作。接收终止信号退出的方式适用于某些特定情况，例如需要远程管理或外部实体控制进程的情况。但即使使用终止信号，也建议在信号处理程序中调用 exit() 函数，以确保资源的正确释放。

# PART 2

``` c
/* Complete version of pipeline
    communication experimental program */

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int pid1,pid2; 
int main()
{
    int fd[2];
    char InPipe[5000]; // buffer
    char c1='1', c2='2';
    pipe(fd); 

    while((pid1 = fork( )) == -1);
    if(pid1 == 0) { // Child1:
        lockf(fd[1],1,0);
        for(int i = 0 ; i < 2000; i++)
            write(fd[1], &c1, 1); //Write '1'
        sleep(5);
        lockf(fd[1],0,0);
        exit(0);
    }
    else {
        while((pid2 = fork()) == -1); 
        if(pid2 == 0) { // Child2:
            lockf(fd[1],1,0);
            for(int i = 0 ; i < 2000; i++)
                write(fd[1], &c2, 1); // Write '2'
            sleep(5);
            lockf(fd[1],0,0);
            exit(0);
        }
        else { // Father:
            int status;
            wait(NULL);
            wait(NULL);
            //sleep(2);
            int end = read(fd[0], &InPipe, 4000); // Read
            InPipe[end] = '\0'; 
            printf("%s\n",InPipe); 
            exit(0); 
        }
    }
}
```

## 1、预期运行结果

**加锁**:显示由连续的'1'和连续的'2'组成的字符串。  
**不加锁**：显示'1'和'2'随机出现的字符串。

## 2、实际结果以及对结果的分析

实际结果同预期结果一致。见下图：  
加锁  
![Alt text](img/img4.png)  
不加锁    
![Alt text](img/img5.png)  
特点及原因:

* 加锁时是写入的'1'和'2'是连续的。这是因为子进程1和子进程2在写入管道时使用 lockf 函数进行加锁，以确保一次只有一个子进程能够写入数据。这样实现了连续的写入。

* 不加锁时二者随机出现。这是因为两个程序同时写入，写入顺序随机。

## 3、同步与互斥   

同步通过wait实现。这确保了写程序完成后读程序才开始读，二者之间的同步关系是保证顺利完成读写必要条件。此例中另一种确保同步的方式可以是在父进程中用sleep等待。   
互斥通过子进程在写入管道时使用 lockf 函数来实现。两个子程序都试图锁定管道，但只有一个子进程可以成功锁定，另一个会被阻塞。这确保了在任何时刻只有一个子进程能够写入数据，防止了竞争条件和数据混乱。  
如果不控制同步与互斥，对文件的写入和读取就会不可控。

# PART 3

## 1、 FF、 BF、 WF 策略及实现思路

**首次适应(first-fit)**：将空闲内存链表按开始地址从小到大排序，从链表头开始查找，分配首个足够大的孔。

**最优适应(best-fit)**：将空闲内存链表按大小从小到大排序，从链表头开始查找，分配首个足够大的孔。

**最差适应(worst-fit)**：将空闲内存链表按大小从大到小排序，从链表头开始查找，分配首个足够大的孔。

## 2、算法对比

* **首次适应算法**倾向于优先利用内存中低址部分的空闲分区，从而保留了高址部分的大空闲区。这给为以后到达的大作业分配大的内存空间创造了条件。其缺点是低址部分不断被划分，会留下许多难以利用的、很小的空闲分区，而每次查找又都是从低址部分开始，这无疑会增加查找可用空闲分区时的开销。

* **最佳适应算法**总是把能满足要求、又是最小的空闲分区分配给作业，避免“大材小用”，但因为每次分配后所切割下来的剩余部分总是最小的，这样，在存储器中会留下许多难以利用的小空闲区。

* **最坏适应分配算法**总是挑选一个最大的空闲区分割给作业使用，其优点是可使剩下的空闲区不至于太小，产生碎片的几率小，对中、小作业有利，同时最坏适应分配算法查找效率很高。但是该算法会使存储器中缺乏大的空闲分区。

# 3、一次实例

创建进程如下  
![Alt text](img/img6.png)  
删去2、5号进程  
![Alt text](img/img7.png)  
改用BF算法  
![Alt text](img/img8.png)  
创建一个新进程，大小为8  
![Alt text](img/img9.png)  
改用WF算法  
![Alt text](img/img10.png)  

```c
/**
*****************************************************************************
*  Copyright (C), 2023.11
*
*  @ Memory processing simulation program 
*  FUNCTION LIST :
*    1 - Set memory size (default=1024)
*    2 - Select memory allocation algorithm
*    3 - New process
*    4 - Terminate a process
*    5 - Display memory usage
*    0 - Exit
*****************************************************************************
*/

#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

/*指向内存中空闲块链表的首指针*/
struct free_block_type *free_block;
/*进程分配内存块链表的首指针*/
struct allocated_block *allocated_block_head = NULL;

int mem_size = DEFAULT_MEM_SIZE; /*内存大小*/
int ma_algorithm = MA_FF;        /*当前分配算法*/
static int pid = 0;              /*初始 pid*/
int flag = 0;                    /*设置内存大小标志*/

int main()
{
    char choice;
    pid = 0;
    free_block = init_free_block(mem_size); // 初始化空闲区
    while(1) {
        display_menu(); //显示菜单
        fflush(stdin);
        choice = getchar(); // 获取用户输入
        switch(choice){
            case '1': set_mem_size(); break;                //设置内存大小
            case '2': set_algorithm(); flag = 1; break;     // 设置算法
            case '3': new_process(); flag = 1; break;       // 创建新进程
            case '4': kill_process(); flag = 1; break;      // 删除进程
            case '5': display_mem_usage(); flag =1 ; break; // 显示内存使用
            case '0': do_exit(); exit(0);                   //释放链表并退出
            default: break; 
        } 
    }
}

/*初始化空闲块，默认为一块，可以指定大小及起始地址*/
struct free_block_type* init_free_block(int mem_size){
    struct free_block_type *fb;
    fb = (struct free_block_type *)malloc(sizeof(struct free_block_type));
    if (fb == NULL)
    {
        printf("No mem\n");
        return NULL;
    }
    fb->size = mem_size;
    fb->start_addr = DEFAULT_MEM_START;
    fb->next = NULL;
    return fb;
}

/*显示菜单*/
void display_menu(){
    printf("\n");
    printf("1 - Set memory size (default=%d)\n", DEFAULT_MEM_SIZE);
    printf("2 - Select memory allocation algorithm\n");
    printf("3 - New process \n");
    printf("4 - Terminate a process \n");
    printf("5 - Display memory usage \n");
    printf("0 - Exit\n");
}

/*设置内存的大小*/
int set_mem_size(){
    int size;
    if (flag != 0)
    { // 防止重复设置
        printf("Cannot set memory size again\n");
        return 0;
    }
    printf("Total memory size =");
    scanf("%d", &size);
    if (size > 0)
    {
        mem_size = size;
        free_block->size = mem_size;
    }
    flag = 1;
    return 1;
}

/* 设置当前的分配算法 */
void set_algorithm(){
    int algorithm;
    printf("\t1 - First Fit\n");
    printf("\t2 - Best Fit \n");
    printf("\t3 - Worst Fit \n");
    scanf("%d", &algorithm);
    if (algorithm >= 1 && algorithm <= 3)
        ma_algorithm = algorithm;
    // 按指定算法重新排列空闲区链表
    rearrange(ma_algorithm);
}

/*按指定的算法整理内存空闲块链表*/
void rearrange(int algorithm){  
    switch(algorithm){
        case MA_FF: rearrange_FF(); break;
        case MA_BF: rearrange_BF(); break;
        case MA_WF: rearrange_WF(); break;
    }
}

/*按 FF 算法重新整理内存空闲块链表*/
void rearrange_FF(){
    struct free_block_type *fbt,*work, *minblock;
    work = free_block;
    if (work->next == NULL)
        return;
   
    // Sort by starting address
    while (work)
    {
        fbt = work;
        int min = 0x7FFFFFFF;
        while (fbt){
            if (fbt->start_addr < min)
            {
                min = fbt->start_addr;
                minblock = fbt;
            }
            fbt = fbt->next;
        }
        int temp_size, temp_addr;
        temp_size = work->size;
        temp_addr = work->start_addr;
        work->size = minblock->size;
        work->start_addr = minblock->start_addr;
        minblock->size = temp_size;
        minblock->start_addr = temp_addr;
        work = work->next;
    }
}

/*按 BF 算法重新整理内存空闲块链表*/
void rearrange_BF(){
    struct free_block_type *fbt,*work, *minblock;
    work = free_block;
    if (work->next == NULL)
        return;
    
    // Sort from smallest to largest based on memory size
    while (work)
    {
        fbt = work;
        int min = 0x7FFFFFFF;
        while (fbt){
            if (fbt->size < min)
            {
                min = fbt->size;
                minblock = fbt;
            }
            fbt = fbt->next;
        }
        int temp_size, temp_addr;
        temp_size = work->size;
        temp_addr = work->start_addr;
        work->size = minblock->size;
        work->start_addr = minblock->start_addr;
        minblock->size = temp_size;
        minblock->start_addr = temp_addr;
        work = work->next;
    }
}

/*按 WF 算法重新整理内存空闲块链表*/
void rearrange_WF(){
    struct free_block_type *fbt,*work, *maxblock;
    work = free_block;
    if (work->next == NULL)
        return;
    
    // Sort from smallest to largest based on memory size
    while (work)
    {
        fbt = work;
        int max = -1;
        while (fbt){
            if (fbt->size > max)
            {
                max = fbt->size;
                maxblock = fbt;
            }
            fbt = fbt->next;
        }
        int temp_size, temp_addr;
        temp_size = work->size;
        temp_addr = work->start_addr;
        work->size = maxblock->size;
        work->start_addr = maxblock->start_addr;
        maxblock->size = temp_size;
        maxblock->start_addr = temp_addr;
        work = work->next;
    }
}

/*创建新的进程，主要是获取内存的申请数量*/
int new_process(){
    struct allocated_block *ab;
    int size;
    int ret;
    ab = (struct allocated_block *)malloc(sizeof(struct allocated_block));
    if (!ab) exit(-5);
    ab->next = NULL;
    pid++;
    sprintf(ab->process_name, "PROCESS-%02d", pid);
    ab->pid = pid;
    printf("Memory for %s:", ab->process_name);
    scanf("%d", &size);
    if (size > 0)
        ab->size = size;
    ret = allocate_mem(ab); /* 从空闲区分配内存， ret==1 表示分配 ok*/
    /*如果此时 allocated_block_head 尚未赋值，则赋值*/
    if ((ret == 1) && (allocated_block_head == NULL))
    {
        allocated_block_head = ab;
        return 1;
    }
    /*分配成功，将该已分配块的描述插入已分配链表*/
    else if (ret == 1){
        ab->next = allocated_block_head;
        allocated_block_head = ab;
        return 2;
    }
    else if(ret == -1){ /*分配不成功*/
        printf("Allocation fail\n");
        free(ab);
        return -1;
    }
    return 3;
}

/*分配内存模块*/
int allocate_mem(struct allocated_block *ab){
    struct free_block_type *fbt, *pre;
    int request_size = ab->size;
    fbt = pre = free_block;
    while (fbt)
    {
        if(fbt == NULL)
            return -1;
        if(fbt->size >= request_size){ // Here is the block
            ab->start_addr = fbt->start_addr;
            
            // cut free block
            if(fbt->size == request_size){
                
                if(pre == fbt && fbt->next == NULL){ // first and the only free block deleted
                    fbt->size = 0;
                }
                else if(pre == fbt && fbt->next){ // first but not the only free block deleted
                    free_block = fbt->next;
                    free(fbt);
                }
                else{ 
                    pre->next = fbt->next;
                    free(fbt);
                }
            }
            else{
                fbt->start_addr += request_size;
                fbt->size -= request_size;
            }
            rearrange(ma_algorithm);
            return 1;
        }
        pre = fbt;
        fbt = fbt->next;
    }
}

/*删除进程，归还分配的存储空间，并删除描述该进程内存分配的节点*/
void kill_process(){
    struct allocated_block *ab;
    int pid;
    printf("Kill Process, pid=");
    scanf("%d", &pid);
    ab = find_process(pid);
   
    if (ab != NULL)
    {
        free_mem(ab); /*释放 ab 所表示的分配区*/
        dispose(ab);  /*释放 ab 数据结构节点*/
    }
}

struct allocated_block *find_process(int pid){
    struct allocated_block *ab = allocated_block_head;
    while(ab){
        if(ab->pid == pid) 
            return ab;
        ab = ab->next;
    }
    return NULL;
}

/*将 ab 所表示的已分配区归还，并进行可能的合并*/
int free_mem(struct allocated_block *ab){
    int algorithm = ma_algorithm;
    struct free_block_type *fbt, *pre, *work;
    fbt = (struct free_block_type *)malloc(sizeof(struct free_block_type));
    if (!fbt)
        return -1;
    fbt->start_addr = ab->start_addr;
    fbt->size = ab->size;

    // Put new block in the end
    work = free_block;
    if(work){
        while(work->next){
            work = work->next;
        }
        work->next = fbt;
    }
    else{
        free_block = fbt;
    }
    rearrange_FF();

    // Merge adjacent regions
    pre = free_block;
    work = free_block->next;
    while(work){
        if(pre->start_addr + pre->size == work->start_addr){
            pre->size += work->size;
            pre->next = work->next;
            free(work);
            work = pre->next;
        }
        else{
            pre = work;
            work = work->next;
        }
    }

    rearrange(ma_algorithm);
    return 1;
}

/*释放 ab 数据结构节点*/
int dispose(struct allocated_block *free_ab){
    struct allocated_block *pre, *ab;
    if (free_ab == allocated_block_head)
    { /*如果要释放第一个节点*/
        allocated_block_head = allocated_block_head->next;
        free(free_ab);
        return 1;
    }
    pre = allocated_block_head;
    ab = allocated_block_head->next;
    while (ab != free_ab)
    {
        pre = ab;
        ab = ab->next;
    }
    pre->next = ab->next;
    free(ab);
    return 2;
}

/* 显示当前内存的使用情况，包括空闲区的情况和已经分配的情况 */
int display_mem_usage(){
    struct free_block_type *fbt = free_block;
    struct allocated_block *ab = allocated_block_head;
    if (fbt == NULL)
        return (-1);
    printf("----------------------------------------------------------\n"); /* 显示空闲区 */
    printf("Free Memory:\n");
    printf("%20s %20s\n", " start_addr", " size");
    while (fbt != NULL)
    {
        printf("%20d %20d\n", fbt->start_addr, fbt->size);
        fbt = fbt->next;
    }
    /* 显示已分配区 */
    printf("\nUsed Memory:\n");
    printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "start_addr", " size");
    while (ab != NULL)
    {
        printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name,
               ab->start_addr, ab->size);
        ab = ab->next;
    }
    printf("----------------------------------------------------------\n");
    return 0;
}

void do_exit(){
    struct free_block_type *ftb, *del;
    struct allocated_block *alb, *dela;
    ftb = free_block;
    alb = allocated_block_head;
    while (ftb){
        del = ftb;
        ftb = ftb->next;
        free(del);
    }
    while(alb){
        dela = alb;
        alb = alb->next;
        free(dela);
    }
}
``` 
<men.h>
```c
#ifndef MEM_H 
#define MEM_H 

#define PROCESS_NAME_LEN 32   /*进程名长度*/
#define PROCESS_NAME_LEN 32   /*进程名长度*/
#define MIN_SLICE 10          /*最小碎片的大小*/
#define DEFAULT_MEM_SIZE 1024 /*内存大小*/
#define DEFAULT_MEM_START 0   /*起始位置*/

#define MA_FF 1
#define MA_BF 2
#define MA_WF 3

/*描述每一个空闲块的数据结构*/
struct free_block_type{
    int size;
    int start_addr;
    struct free_block_type *next;
};

/*每个进程分配到的内存块的描述*/
struct allocated_block{
    int pid;
    int size;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    struct allocated_block *next;
};

struct free_block_type *init_free_block(int mem_size);
int set_mem_size();
void set_algorithm();
int new_process();
void kill_process();
int display_mem_usage();
void do_exit();

/* Memory algorithm */
void rearrange(int algorithm);
int allocate_mem(struct allocated_block *ab);
struct allocated_block *find_process(int pid);
int free_mem(struct allocated_block *ab);
int dispose(struct allocated_block *free_ab);
void rearrange_FF();
void rearrange_BF();
void rearrange_WF();

/* Menu function */
void display_menu();

#endif 
```