#include "stat_server.h"

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

int get_shared_segment(int key) {
  long page_size = getPageSize();
  int shmid = shmget(key, page_size, 0);
  if (shmid < 0) {
    perror("Error: Failed to get shared mem segment.");
    exit(1);
  }
  return shmid;
}

int isInteger(char *p) {
  int len = strlen(p);
  int i;
  for (i = 0 ; i < len; i++) {
    if (!isdigit(p[i])) {
      return -1;
    }
  }
  return 0;
}

int parseKey(int *key, char *keystr) {
  if (isInteger(keystr) == 0) {
    *key = atoi(keystr);
    return 0;
  }
  return -1;
}

long getPageSize(void) {
  return sysconf(_SC_PAGESIZE);
}
