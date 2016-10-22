#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  sem_t *sem1 = sem_open(argv[1], O_CREAT, 0644, 1);
  printf("Got semaphore\n");
  
  if (SEM_FAILED == sem1) {
    perror("Error");
    exit(1);
  }

  sleep(5);

  int ret = sem_wait(sem1);
  if (ret == -1) {
    perror("Error");
    exit(1);
  }
  printf("wait complete for semaphore\n");

  /* ret = sem_post(sem1); */
  /* if (ret == -1) { */
  /*   perror("Error"); */
  /*   exit(1); */
  /* } */

  // printf("posted semaphore\n");

  ret = sem_close(sem1);
  if (ret == -1) {
    perror("Error");
    exit(1);
  }

  ret = sem_unlink(argv[1]);
  if (ret == -1) {
    perror("Error");
    exit(1);
  }

  
  printf("closed semaphore\n");
  //sleep(25);
  return 0;
}
