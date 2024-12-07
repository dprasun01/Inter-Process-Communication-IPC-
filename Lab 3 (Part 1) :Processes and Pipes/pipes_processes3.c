#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/**
 * Lab 3 (Part 1) Solution
 */

int main(int argc, char **argv)
{
  // first pipe and pid
  int pipefd[2];
  int pid;

  // second pipe and pid
  int pipefd2[2];
  int pid2;

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};

  if (argc != 2) {
    exit(1);
  };

  // make a pipe for P1 and P2 (fds go in pipefd[0] and pipefd[1])

  pipe(pipefd);

  pid = fork();

  if (pid == 0)
    {
      // child gets here and handles grep or sort command

      pipe(pipefd2);

      pid2 = fork();

      if (pid2 == 0) {

        // P3 executes the sort command

        // replace standard input with input part of pipe2
        dup2(pipefd2[0], 0);

        // close unused half of pipe2
        close(pipefd2[1]);

        // close unused ends of pipe
        close(pipefd[0]);
        close(pipefd[1]);

        // execute sort
        execvp("sort", sort_args);

      }

      else {

        // P2 executes the grep command

        // replace standard input with input part of pipe
        dup2(pipefd[0], 0);

        // close unused half of pipe
        close(pipefd[1]);

        // replace standard output with output part of pipe2
        dup2(pipefd2[1], 1);

        // close unused half of pipe2
        close(pipefd2[0]);

        // execute grep
        execvp("grep", grep_args);
        
      };
    }
  else
    {
      // parent P1 gets here and handles "cat scores"

      // replace standard output with output part of pipe

      dup2(pipefd[1], 1);

      // close unused unput half of pipe

      close(pipefd[0]);

      // execute cat

      execvp("cat", cat_args);

      exit(0);
    };
}