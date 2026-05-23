/*
 * meow.c
 * simple "victim" process for 
 * injection testing
 * author @cocomelonc
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  printf("victim process started. PID: %d\n", getpid());

  while (1) {
    printf("meow-meow... PID: %d\n", getpid());
    sleep(5); // simulate periodic activity
  }

  return 0;
}
