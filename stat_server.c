#include "stat_server.h"

int main(int argc, char *argv[]) {
  int key = -1;

  if (argc != 3) {
    fprintf(stderr, "Usage: stat_server -k key\n");
    exit(1);
  }

  //Parse & extract key from command line opts
  int c;
  while ((c = getopt(argc, argv, "k:")) != -1) {
    switch (c) {
    case 'k':
      if (parseKey(&key, optarg) < 0) {
	fprintf(stderr, "Error: Invalid key %s\n", optarg);
	exit(1);
      }
      break;
    default:
      fprintf(stderr, "Usage: stat_server -k key\n");
      exit(1);
    }
  }
  
  int shmid = create_shared_segment(key);

  printf("shmid = %d\n", shmid);
  
  return 0;
}

int create_shared_segment(int key) {
  int shmid = shmget(key, 4096, IPC_CREAT | IPC_EXCL | 0666);
  if (shmid < 0) {
    perror("Error: Failed to get shared mem segment.");
    exit(1);
  }
  
  return shmid;
}
