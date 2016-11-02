#include "stats_server.h"

// Metadata struct to hold segment initial data
segment_meta_t *shared_seg = NULL;

// Struct to hold shared segment access info
ssegment_t ssegment;
ssegment_t *shm;

// Semaphore used by clients
sem_t *semaphore;
char sem_name[1024];

int main(int argc, char *argv[]) {
  // Set up SIGINT handler
  struct sigaction act;
  init_s_handler(&act);
  int server_count, inuse;
  server_count = inuse = 0;

  int key = -1;
  if (argc != 3) {
    fprintf(stderr, "Usage: stats_server -k key\n");
    exit(1);
  }

  // Parse & extract key from command line opts
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
      fprintf(stderr, "Usage: stats_server -k key\n");
      exit(1);
    }
  }

  if (key == -1) {
    fprintf(stderr, "Usage: stats_server -k key\n");
    exit(1);
  }

  ssegment.shmid = create_shared_segment(key);
  // printf("shmid = %d\n", ssegment.shmid);

  ssegment.addr = attach_shared_segment(ssegment.shmid);
  // printf("attached to segment\n");

  shared_seg = initialize_segment(ssegment.addr);

  semaphore = init_semaphore(key);
  get_semaphore_name(key, sem_name);

  sem_post(semaphore);

  while (TRUE) {
    inuse = read_segment(shared_seg, server_count);
    if (inuse == 1) {
      server_count++;
      fprintf(stdout, "\n");
    }
    sleep(1);
  }

  return 0;
}

void sigint_handler(int signum) {
  // printf("in signal handler\n");
  cleanup();
  exit(0);
}

void cleanup() {
  /*segment_meta_t *s_seg = (segment_meta_t *) shm;
  int i;
  for (i = 0; i < MAX_CLIENT_COUNT; i++) {
    s_seg->client_status[i] = 0;
  }*/

  if (NULL != shared_seg) {
    /*printf("Detaching shared segment\n");
    //  detach segment
    if (shmdt(ssegment.addr) < 0) {
      perror("Error");
      exit(1);
    }*/

    // printf("Deleting shared segment\n");
    // delete segment
    if (shmctl(ssegment.shmid, IPC_RMID, NULL) < 0) {
      perror("Error");
      exit(1);
    }
  }

  if (NULL != semaphore) {
    /*printf("Closing semaphore\n");
    // unlink semaphore
    if (sem_close(semaphore) < 0) {
      perror("Error");
      exit(1);
    }*/

    // printf("Deleting semaphore\n");
    // unlink semaphore
    if (sem_unlink(sem_name) < 0) {
      perror("Error");
      exit(1);
    }
  }
}

void init_s_handler(struct sigaction *act) {
  act->sa_handler = sigint_handler;
  sigaction(SIGINT, act, NULL);
}


sem_t* init_semaphore(int key) {
  get_semaphore_name(key, sem_name);
  sem_t *semaphore = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 1);
  if (SEM_FAILED == semaphore) {
    perror("Error");
    exit(1);
  }
  return semaphore;
}
