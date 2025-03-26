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
    char * argv3[] = {"wc", "-l", 0};

    // Disable buffering for stdout
    setbuf(stdout, NULL);

    int pipe_fd[2];
    int pipe_fd2[2];
    pid_t pid1, pid2, pid3;

    // Create a pipe to connect child 1 to child 2
    if (pipe(pipe_fd) == -1) {
        printf("Failed to create the pipe");
        exit(1);
    }

    // Create a pipe to connect child 2 to child 3
    if (pipe(pipe_fd2) == -1) {
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
        // Output status before the execvp because it will be replaced
        printf("In CHILD-1 (PID=%d): executing command %s ...\n", getpid(), argv1[0]);

        // Redirect stdout to the write end of the pipe
        // Output of child1 will be written to the pipe
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

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
        // Output status before the execvp because it will be replaced
        printf("In CHILD-2 (PID=%d): executing command %s ...\n", getpid(), argv2[0]);

        // Child 2: head
        // Redirect stdin to the read end of the pipe
        // child 2 will read from the pipe
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);


        // Redirect stdout to the write end of the pipe2
        // Output of child2 will be written to the pipe2
        dup2(pipe_fd2[1], STDOUT_FILENO);
        close(pipe_fd2[0]);
        close(pipe_fd2[1]);

        execvp(argv2[0], argv2);

        // If execvp fails, print error message and exit
        printf("Failed to execvp for head");
        exit(1);
   }


    // Third child (wc)
    if ((pid3 = fork()) == -1) {
        printf("Failed to fork child 3");
        exit(1);
    }
    
    if (pid3 == 0) {
        // Output status before the execvp because it will be replaced
        printf("In CHILD-3 (PID=%d): executing command %s ...\n", getpid(), argv3[0]);

        // Child 2: head
        // Redirect stdin to the read end of the pipe
        // child 2 will read from the pipe
        dup2(pipe_fd2[0], STDIN_FILENO);
        close(pipe_fd2[0]);
        close(pipe_fd2[1]);
 

        execvp(argv3[0], argv3);
 
        // If execvp fails, print error message and exit
        printf("Failed to execvp for wc");
        exit(1);
    }
   
    // Parent

    // Close the pipe in the parent
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    close(pipe_fd2[0]);
    close(pipe_fd2[1]);

   // wait for 3 children to finish
    waitpid(pid1, NULL, 0);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid1);

    waitpid(pid2, NULL, 0);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid2);

    waitpid(pid3, NULL, 0);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid3);
   
   return 0;
}
