#include "../stats_server.h"

void sigint_handler(int signum) {
    printf("in signal handler\n");
    exit(0);
}

void init_s_handler(struct sigaction *act) {
    act->sa_handler = sigint_handler;
    sigaction(SIGINT, act, NULL);
  }
int
main(int argc, char *argv[]) {
  struct sigaction act;
  init_s_handler(&act);
  printf("Sleeping!\n");
  sleep(10);
  printf("Done\n");
  return 0;
}
