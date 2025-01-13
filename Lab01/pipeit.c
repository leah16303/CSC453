
#include "pipeit.h"
/*The program pipeit will list all the files and directories*/
/* in our current directory and reverse them and put that */
/*result into a file named "outfile"*/

/*main calls the ls and sort function after forking*/
int main(){
    int fd[2], status;
    pid_t pid, pid2;

    /*create pipe*/
    if (pipe(fd) < 0) {
        perror("pipe error"); /*error if pipe failed*/
        exit(EXIT_FAILURE);
    }
    /*fork to handle ls*/
    if ((pid = fork()) < 0) { 
        perror("fork error"); /*error if fork failed*/
        exit(EXIT_FAILURE);
    } 
    if (pid == 0) {  /*child #1 - runs ls */ 
      ls(fd);
    } 

    /*fork to handle ls*/
    if ((pid2 = fork()) < 0) { 
        perror("fork error"); /*error if fork failed*/
        exit(EXIT_FAILURE);
    }
     if (pid2 == 0) { /*child #2 - runs sort and reverse */
      sort(fd);
    }

    /*parent cloes read and write fd*/
    close(fd[0]);
    close(fd[1]);

    /*wait for ls and sort to finish*/
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid ls");
        exit(EXIT_FAILURE);
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "ls process failed\n");
        exit(EXIT_FAILURE);
    }

    if (waitpid(pid2, &status, 0) == -1) {
        perror("waitpid sort");
        exit(EXIT_FAILURE);
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "sort process failed\n");
        exit(EXIT_FAILURE);
    }

    /*If ls and sort sucseed exit success*/
    return 0;
}/*end of main*/


/*Runs ls and pipes the output*/
void ls(int* fd){
  char *args[] = { "ls", (char*) 0 };
  close(fd[0]); /*close read end*/

  /*make stdout write to pipe*/
  if (dup2(fd[1], STDOUT_FILENO) < 0){
    perror("dup2 error\n"); /*error if dup2 fails*/
    exit(EXIT_FAILURE);
  }
  

  /*call ls*/
  if (execv("/bin/ls", args) < 0) {
        perror("execv error\n"); /*error if exec fails*/
        exit(EXIT_FAILURE);
    }

  perror("ls never called\n");
  exit(EXIT_FAILURE);
}


/*Runs sort and reverse*/
void sort(int *fd){
  char *args[] = {"/bin/sh", "-c", "sort -r", (char*) 0 };

  close(fd[1]); /*close write end*/

  /*Make stdin read from pipe*/
  if (dup2(fd[0], STDIN_FILENO) < 0){
    perror("dup2 error\n"); 
    exit(EXIT_FAILURE);
  }
  int output = open("outfile", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (output < 0){
    perror("open error\n");
    exit(EXIT_FAILURE);
  }

  if (dup2(output, STDOUT_FILENO) < 0){
    perror("dup2 error\n");
    exit(EXIT_FAILURE);
  }
  
  close(output); /*close output file*/
  

  /*call sort, with reverse, and put in file names "outfile"*/
  if (execvp(args[0], args) < 0) {
        perror("execv error\n"); /*error if exec fails*/
        exit(EXIT_FAILURE);
    }

  perror("sort never called\n");
  exit(EXIT_FAILURE);
}
