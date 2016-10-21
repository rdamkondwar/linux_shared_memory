#include "stat_server.h"

segment_meta_t *shared_seg = NULL;
ssegment_t ssegment;

int main(int argc, char *argv[]) {
  //Set up SIGINT handler
  struct sigaction act;
  init_s_handler(&act);

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

  if (key == -1) {
    fprintf(stderr, "Usage: stat_server -k key\n");
    exit(1);
  }

  ssegment.shmid = create_shared_segment(key);
  printf("shmid = %d\n", ssegment.shmid);

  ssegment.addr = attach_shared_segment(ssegment.shmid);
  printf("attached to segment\n");

  shared_seg = initialize_segment(ssegment.addr);

  while (TRUE) {
    read_segment(shared_seg);
    sleep(1);
  }

  return 0;
}

void read_segment(segment_meta_t *shared_seg) {
  // Read first int
  printf("init = %d\n", shared_seg->init_status);
  // Read MAX_CLIENT_COUNT ints
  int i;
  for (i = 0; i < MAX_CLIENT_COUNT; i++) {
    int val = shared_seg->client_status[i];
    if (val > 0) {
      printf("child %d = %d\n", i+1, val);
    }
  }
}

segment_meta_t* initialize_segment(char *addr) {
  segment_meta_t *shared_seg = (segment_meta_t *)addr;
  // Put 1 as first int
  shared_seg->init_status = 1;
  addr+=sizeof(int);

  int i;
  for (i = 0; i < MAX_CLIENT_COUNT; i++) {
    shared_seg->client_status[i] = 0;
  }
  shared_seg->head = addr + sizeof(segment_meta_t);

  // 2 implies init complete
  shared_seg->init_status = 2;
  return shared_seg;
}

char* attach_shared_segment(int shmid) {
  char *p = (char *)shmat(shmid, NULL, 0);
  if ((char *)-1 == p) {
    perror("shmat: \n");
    exit(1);
  }
  return p;
}

int create_shared_segment(int key) {
  long page_size = getPageSize();
  int shmid = shmget(key, page_size, IPC_CREAT | IPC_EXCL | 0666);
  if (shmid < 0) {
    perror("Error: Failed to get shared mem segment.");
    exit(1);
  }
  return shmid;
}

void sigint_handler(int signum) {
  printf("in signal handler\n");
  cleanup();
  exit(0);
}

void cleanup() {
  if (NULL != shared_seg) {
    printf("Detaching shared segment\n");
    //detach segment
    if (shmdt(ssegment.addr) < 0) {
      perror("Error");
      exit(1);
    }

    printf("Deleting shared segment\n");
    //delete segment
    if (shmctl(ssegment.shmid, IPC_RMID, NULL) < 0) {
      perror("Error");
      exit(1);
    }
  }
}

void init_s_handler(struct sigaction *act) {
  act->sa_handler = sigint_handler;
  sigaction(SIGINT, act, NULL);
}
