#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void signal_handler(int signum) {
  printf("in signal handler\n");
  exit(2);
}

int main(int argc, char* argv[]) {
  struct sigaction act;

  act.sa_handler = signal_handler;
  sigaction(SIGINT, &act, NULL);
  while(1);
}
