/*
 * hack3.c - hashing Win32API functions
 * with MurmurHash2A
 * @cocomelonc
*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

unsigned int MurmurHash2A(const void *input, size_t length, unsigned int seed) {
  const unsigned int m = 0x5bd1e995;
  const int r = 24;
  unsigned int h = seed ^ length;
  const unsigned char *data = (const unsigned char *)input;

  while (length >= 4) {
    unsigned int k = *(unsigned int *)data;
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
    data += 4;
    length -= 4;
  }

  switch (length) {
    case 3:
      h ^= data[2] << 16;
    case 2:
      h ^= data[1] << 8;
    case 1:
      h ^= data[0];
      h *= m;
  };

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
}

int main() {
  DWORD h = MurmurHash2A("WSASocket", strlen("WSASocket"), 0);
  printf("%d\n", h);
  return 0;
}