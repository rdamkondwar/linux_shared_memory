#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  long sz = sysconf(_SC_PAGESIZE);
  printf("Pagesize = %ld\n", sz);
}
