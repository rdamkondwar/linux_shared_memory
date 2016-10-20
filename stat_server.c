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

  // sleep(10);
  char *seg_addr = attach_shared_segment(shmid);
  printf("attached to segment\n");
  initialize_segment(seg_addr);
  printf("init complete\n");
  
  while (1) {
    read_segment(seg_addr);
    sleep(1);
  }
  // sleep(10);
  return 0;
}

void read_segment(char *addr) {
  // Read first int
  printf("init = %d\n", *((int *)addr));
  addr += sizeof(int);
  // Read 16 ints
  int i;
  for (i=0; i < 16; i++) {
    int val = *((int *)addr);
    if (val > 0) {
      printf("child %d = %d\n", i+1, val);
    }
    addr += sizeof(int);
  }
}

void initialize_segment(char *addr) {
  // Put 1 as first int
  int *init_complete = (int *) addr;
  *init_complete = 1;

  addr+=sizeof(int);

  int i;
  for (i=0; i < 16; i++) {
    *((int *)addr) = 0;
    addr += sizeof(int);
  }

  // 2 implies init complete
  *init_complete = 2;
}

char* attach_shared_segment(int shmid) {
  char *p = (char *)shmat(shmid, NULL, 0);
  if ((char *)-1 == p) {
    perror("shmat: \n");
    exit(1);
  }
  // printf("p = %c\n", *p);
  return p;
}

int create_shared_segment(int key) {
  int shmid = shmget(key, 4096, IPC_CREAT | IPC_EXCL | 0666);
  if (shmid < 0) {
    perror("Error: Failed to get shared mem segment.");
    exit(1);
  }
  
  return shmid;
}
