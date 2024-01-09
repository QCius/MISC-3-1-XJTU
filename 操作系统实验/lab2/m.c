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
    int all_free_size = 0;
    fbt = pre = free_block;
    while (fbt)
    {
        if(fbt == NULL)
            return -1;
        all_free_size += fbt->size;
        if (fbt->size >= request_size)
        { // Here is the block
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
    // Not enough memory
    if(all_free_size >= request_size){ // Memory compression
        struct allocated_block *head = allocated_block_head;
        struct free_block_type *fbt_n, *pre_n;
        fbt_n = pre_n = free_block;
        int addr_new = 0;

        while (head)
        {
            head->start_addr = addr_new;
            addr_new += head->size;
            head = head->next;
        }
        ab->start_addr = addr_new;
        addr_new += request_size;

        // There must be over one free block
        fbt_n = fbt_n->next;
        while (fbt_n)
        {
            pre_n = fbt_n;
            fbt_n = fbt_n->next;
            free(pre_n);
        }
        free_block->start_addr = addr_new;
        free_block->size = mem_size - addr_new;
        free_block->next = NULL;

        rearrange(ma_algorithm);
        return 1;
    }
    return -1;
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