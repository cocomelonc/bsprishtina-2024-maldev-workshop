/*
 * Malware Persistence 101
 * heapify.c
 * victim vulnerable application
 * author: @cocomelonc
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;

void run_cmd(char *cmd) {
  pid_t pid;
  char *argv[] = {"sh", "-c", cmd, NULL};
  int status;

  printf("Run command: %s\n", cmd);
  status = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);

  if (status == 0) {
    printf("Child pid: %i\n", pid);
    if (waitpid(pid, &status, 0) != -1) {
      printf("Child exited with status %i\n", status);
    } else {
      perror("waitpid");
    }
  } else {
    printf("posix_spawn: %s\n", strerror(status));
  }
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("\x1B[31mUsage: ./heapify <username>\x1B[0m\n");
    exit(0);
  }

  char *name = malloc(64);
  char *command = malloc(64);

  strcpy(command, "date");

  // here vulnerable: no bounds checking
  strcpy(name, argv[1]);

  printf("\x1B[36mUser: %s is executing command \"%s\"\n", name, command);
  printf("\x1B[0m");

  run_cmd(command);
  return 0;
}