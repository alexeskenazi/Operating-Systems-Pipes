#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    char * argv1[] = {"cat", "Makefile", 0};
    char * argv2[] = {"head", "-4", 0};
    //char * argv2[] = {"wc", "-l", 0};
    
    setbuf(stdout, NULL);

    int pipe_fd[2];
    pid_t pid1, pid2;
    
    // Create a pipe to connect the two children
    if (pipe(pipe_fd) == -1) {
        printf("Failed to create the pipe");
        exit(1);
    }
    
    // First child (cat)
    if ((pid1 = fork()) == -1) {
        printf("Failed to fork child 1");
        exit(1);
    }
    
    if (pid1 == 0) {
        // Child 1: cat
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // Output status before the execvp because it will be replaced
        printf("In CHILD-1 (PID=%d): executing command %s ...\n", getpid(), argv1[0]);
        execvp(argv1[0], argv1);

        // If execvp fails, print error message and exit
        printf("Failed to execvp for cat");
        exit(1);
    }
    
    // Second child (head)
    if ((pid2 = fork()) == -1) {
        printf("Failed to fork child 2");
        exit(1);
    }
    
    if (pid2 == 0) {
        // Child 2: head
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // Output status before the execvp because it will be replaced
        printf("In CHILD-2 (PID=%d): executing command %s ...\n", getpid(), argv2[0]);
        execvp(argv2[0], argv2);

        // If execvp fails, print error message and exit
        printf("Failed to execvp for head");
        exit(1);
    }
    
    // Parent

    // Close the pipe in the parent
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // wait for both children to finish
    waitpid(pid1, NULL, 0);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid1);
    waitpid(pid2, NULL, 0);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid2);
    
    return 0;
}
