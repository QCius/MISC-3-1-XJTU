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