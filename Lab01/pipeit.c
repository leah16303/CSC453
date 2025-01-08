
#include "pipeit.h"
/*The program pipeit will list all the files and directories in our current directory
 and reverse them and put that result into a file named "outfile"*/

/*main calls the ls and sort function after forking*/
int main(){
    int fd[2];
    pid_t pid;


    if (pipe(fd) < 0) {
        perror("pipe error"); /*error if pipe failed*/
    }
    if ((pid = fork()) < 0) { /*error if fork failed*/
        perror("fork error");
    } else if (pid > 0) {  /*parent - runs ls */ 
      ls(fd);
    } else {              /*child - runs sort and reverse */
      sleep(2);
      sort(fd);
    }
    return -1;
}/*end of main*/


/*Runs ls and pipes the output*/
void ls(int* fd){
  char *args[] = { "ls", (char*) 0 };

  if (dup2(fd[1], STDOUT_FILENO) < 0){
    perror("dup2 error\n");
  }
  close(fd[0]); /*close read end*/

  /*call ls*/
  if (execv("/bin/ls", args) < 0) {
        perror("execv error\n"); /*error if exec fails*/
    }

  perror("ls never called\n");
  return; 

}/*end of ls*/


/*Runs sort and reverse*/
void sort(int *fd){
  char *args[] = {"/bin/sh", "-c", "sort -r > outfile", (char*) 0 };
  
  if (dup2(fd[0], STDIN_FILENO) < 0){
    perror("dup2 error\n");
  }
  
  close(fd[1]); /*close write end*/

  /*call sort, with reverse, and put in file names "outfile"*/
  if (execvp(args[0], args) < 0) {
        perror("execv error\n"); /*error if exec fails*/
    }

  perror("sort never called\n");
}
