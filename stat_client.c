#include "stat_server.h"

int main(int argc, char *argv[]) {
  //Get key from argv[1]
  int key = atoi(argv[1]);
  stats_t * c_stats = stat_init((key_t)key);
  c_stats->pid = (int)getpid();
  c_stats->counter = 0;

  while(TRUE) {
    c_stats->counter++;
    printf("Sleeping!\n");
    sleep(1);
  }
}

