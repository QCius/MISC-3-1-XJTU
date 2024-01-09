#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define N 8888

int value = 0;
sem_t mutex; // Declare the semaphore

pid_t gettid(); // Personally encapsulate gettid func
void *runner1(void *param);
void *runner2(void *param);

int main()
{
    pthread_t tid1, tid2;
    pthread_attr_t attr1, attr2;

    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);

    sem_init(&mutex, 0, 1); // Initialize the semaphore with value 1 for mutual exclusion

    pthread_create(&tid1, &attr1, runner1, NULL);
    pthread_create(&tid2, &attr2, runner2, NULL);

    pthread_join(tid1,NULL);
    pthread_join(tid2, NULL);
      
    sem_destroy(&mutex); // Destroy the semaphore when done

    //printf("value = %d\n", value);
}

void *runner1(void *param)
{
    printf("thread1 create success!\n");

    // for (int i = 0; i < N; i++) {
    //     sem_wait(&mutex); // Wait for the semaphore
    //     value += 2;
    //     sem_post(&mutex); // Release the semaphore
    // }
    
    pid_t pid = getpid();
    pthread_t tid = gettid();
    printf("thread1 tid = %ld ,pid = %d\n",tid, pid);
  
    int sys_result = system("./system_call");
    if (sys_result == -1){
        perror("system");
    }
    
    // execlp("./system_call", "system_call", NULL);
    // perror("exec");
    // exit(1);
}

void *runner2(void *param)
{
    printf("thread2 create success!\n");

    // for (int i = 0; i < N; i++) {
    //     sem_wait(&mutex); // Wait for the semaphore
    //     value -= 2;
    //     sem_post(&mutex); // Release the semaphore
    // }

    pid_t pid = getpid();
    pthread_t tid = gettid();
    printf("thread2 tid = %ld ,pid = %d\n",tid, pid);
  
    int sys_result = system("./system_call");
    if (sys_result == -1){
        perror("system");
    }
    
    // execlp("./system_call", "system_call", NULL);
    // perror("exec");
    // exit(1);
}

pid_t gettid()
{
    return syscall(SYS_gettid);
}