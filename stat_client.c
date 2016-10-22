#include "stat_server.h"

sem_t *semaphore;

int main(int argc, char *argv[]) {
  //Get key from argv[1]
  int key = atoi(argv[1]);
  printf("key = %d\n", key);
  //Get semaphore
  semaphore = get_semaphore(key);
  printf("got semaphore\n");
  
  stats_t * c_stats = stat_init((key_t)key);
  printf("Child init done!\n");
  // c_stats->pid = (int)getpid();
  c_stats->counter = 0;

  while(TRUE) {
    c_stats->counter++;
    printf("Sleeping!\n");
    sleep(1);
  }
}

