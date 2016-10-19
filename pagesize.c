#include <unistd.h>
#include <stdio.h>

long getPageSize(void) {
  return sysconf(_SC_PAGESIZE);
}

int main(int argc, char *argv[]) {
  printf("Pagesize = %ld\n", getPageSize());
  return 0;
}
