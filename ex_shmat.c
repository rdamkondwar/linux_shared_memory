#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>

int main(int argc, char *argv[]) {

  int *p = shmat(atoi(argv[1]), NULL, SHM_RND);
  if (0 > p) {
    printf("Error: shmat\n");
    return 1;
  }
  printf("p = %d\n", *p);
  
  return 0;
}
