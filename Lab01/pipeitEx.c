#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define OUTPUT_FILE "outfile"  // Output file to store the sorted result

int main() {
    int pipe_fd[2];  // File descriptors for the pipe
    pid_t pid_ls, pid_sort;

    // Step 1: Create a pipe for inter-process communication
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Step 2: Fork the first child process for the "ls" command
    pid_ls = fork();
    if (pid_ls == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid_ls == 0) {  // Child process for "ls"
        // Step 3: Close the unused write end of the pipe
        close(pipe_fd[0]);

        // Step 4: Redirect stdout to the pipe
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 for ls");
            exit(EXIT_FAILURE);
        }

        // Step 5: Execute "ls" command
        execlp("ls", "ls", NULL);
        perror("execlp ls");  // execlp will only return if an error occurs
        exit(EXIT_FAILURE);
    }

    // Step 6: Fork the second child process for the "sort" command
    pid_sort = fork();
    if (pid_sort == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid_sort == 0) {  // Child process for "sort"
        // Step 7: Close the unused write end of the pipe
        close(pipe_fd[1]);

        // Step 8: Redirect stdin to read from the pipe
        if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
            perror("dup2 for sort");
            exit(EXIT_FAILURE);
        }

        // Step 9: Open the output file for writing
        int out_fd = open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd == -1) {
            perror("open output file");
            exit(EXIT_FAILURE);
        }

        // Step 10: Redirect stdout to the output file
        if (dup2(out_fd, STDOUT_FILENO) == -1) {
            perror("dup2 output file");
            exit(EXIT_FAILURE);
        }
        close(out_fd);  // File descriptor is no longer needed

        // Step 11: Execute "sort -r" command
        execlp("sort", "sort", "-r", NULL);
        perror("execlp sort");  // execlp will only return if an error occurs
        exit(EXIT_FAILURE);
    }

    // Step 12: Close both ends of the pipe in the parent
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // Step 13: Wait for the child processes to finish
    int status;
    if (waitpid(pid_ls, &status, 0) == -1) {
        perror("waitpid ls");
        exit(EXIT_FAILURE);
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "ls process failed\n");
        exit(EXIT_FAILURE);
    }

    if (waitpid(pid_sort, &status, 0) == -1) {
        perror("waitpid sort");
        exit(EXIT_FAILURE);
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "sort process failed\n");
        exit(EXIT_FAILURE);
    }

    // Step 14: If everything succeeded, terminate with status 0
    printf("Pipeline executed successfully. Output written to %s.\n", OUTPUT_FILE);
    return 0;
}
