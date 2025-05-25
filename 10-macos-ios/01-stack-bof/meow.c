/*
 * Malware Persistence 101
 * hello.c (meow.c)
 * "hello, prishtina!" victim
 * author: @cocomelonc
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void secret() {
  printf("we are inside the secret function! meow =^..^=\n");
  exit(0);
}

int main(int argc, char* argv[]) {
  printf("meow test app\n");
  printf("please type your name:\n");

  char buff[16];
  gets(buff);

  printf("hello, %s\n", buff);

  return 0;

}