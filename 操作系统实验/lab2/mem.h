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