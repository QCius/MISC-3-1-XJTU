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