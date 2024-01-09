#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int v = 0;
pid_t system_call();

int main()
{
    pid_t pid, pid1;
    /* fork a child process */
    pid = fork();
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0) { /* child process */
        pid1 = getpid();
        printf("Child process PID: %d", pid1);
        // int sys_result = system("./system_call");
        // if (sys_result == -1) {
        //     perror("system");
        // }
        execlp("./system_call", "system_call", NULL);
        perror("exec");
        exit(1);
        // v += 2;
        // printf("child v = %d", v);
        // printf("child v address = %d", &v);
        //printf("child: pid = %d",pid); /* A */
        //printf("child: pid1 = %d",pid1); /* B */
    }
    else { /* parent process */
        pid1 = getpid();
        printf("Child process PID from parent: %d", pid);
        printf("Parent process PID: %d", pid1);
        // v += 10;
        // printf("parent v = %d", v);
        // printf("parent v address = %d", &v);
        //printf("parent: pid = %d",pid); /* C */
        //printf("parent: pid1 = %d",pid1); /* D */
        wait(NULL);
    }
    //v *= 2;
    //printf("before return v = %d\n", v);
    return 0;
}