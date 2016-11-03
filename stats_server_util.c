#include "stats_server.h"

// extern char sem_name[];
sem_t *semaphore;
extern ssegment_t *shm;
int client_index;
char sem_name[1024];

int read_segment(segment_meta_t *shared_seg, int server_iter) {
  // Read first int
  // printf("init = %d\n", shared_seg->init_status);
  // Read MAX_CLIENT_COUNT ints
  int i;
  int inuse = 0;
  for (i = 0; i < MAX_CLIENT_COUNT; i++) {
    int val = shared_seg->client_status[i];
    if (val > 0) {
      inuse = 1;
      // printf("child %d = %d\n", i+1, val);
      // [server_iter] [pid] [argv[0]] [counter] [cpu_secs] [priority]
      printf("%d %d %s %d %.2lf %d\n", server_iter,
             shared_seg->client_data[i].pid,
             shared_seg->client_data[i].process_name,
             shared_seg->client_data[i].counter,
             shared_seg->client_data[i].cpu_secs,
             shared_seg->client_data[i].priority);
    }
  }
  return inuse;
}

segment_meta_t* initialize_segment(char *addr) {
  segment_meta_t *shared_seg = (segment_meta_t *)addr;
  // Put 1 as first int
  shared_seg->init_status = 1;
  // addr+=sizeof(int);

  int i;
  for (i = 0; i < MAX_CLIENT_COUNT; i++) {
    shared_seg->client_status[i] = 0;
  }

  // 2 implies init complete
  shared_seg->init_status = 2;
  return shared_seg;
}

char* attach_shared_segment(int shmid) {
  char *p = (char *)shmat(shmid, NULL, 0);
  if ((char *)-1 == p) {
    // printf("Failed to attach shared memory\n");
    perror("shmat: \n");
    exit(1);
  }
  return p;
}

/*void
detach_shared_segment(ssegment_t* shm_ptr) {
  int det_ret = shmdt(shm_ptr);
  if (det_ret == -1) {
    perror("Couldn't detach \n");
    exit(1);
  }
}*/

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
    if ('.' != p[i] && !isdigit(p[i])) {
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

int parseLongArg(long *arg, char *argstr) {
  if (isInteger(argstr) == 0) {
    *arg = atol(argstr);
    return 0;
  }
  return -1;
}

long getPageSize(void) {
  return sysconf(_SC_PAGESIZE);
}

void get_semaphore_name(int key, char *sem_name) {
  // Issue: Assume large enough name
  sprintf(sem_name, "rdamkondwar_sharath_%d", key);
  // printf("debug: sem_name=%s\n", sem_name);
}

// Invoked by client
void get_semaphore(int key) {
  char sem_name[MAX_KEY_LENGTH];
  get_semaphore_name(key, sem_name);
  semaphore = sem_open(sem_name, 0);
  if (SEM_FAILED == semaphore) {
    perror("Error");
    exit(1);
  }
}

/* void init_semaphore(int key) { */
/*   char sem_name[MAX_KEY_LENGTH]; */
/*   get_semaphore_name(key, sem_name); */
/*   semaphore = sem_open(sem_name, O_CREAT | O_EXCL, S_IWUSR | S_IRUSR, 1); */
/*   if (SEM_FAILED == semaphore) { */
/*     perror("Error"); */
/*     exit(1); */
/*   } */
/* } */


stats_t* stat_init(key_t key) {
  int shmid;
  char *addr;
  segment_meta_t *s_seg;

  get_semaphore(key);

  // Acquire lock
  if (sem_wait(semaphore) < 0) {
    fprintf(stderr, "Sem_wait failed!\n");
    return NULL;
  }
  shmid = get_shared_segment(key);
  // printf("Debug: shmid %d\n", shmid);
  addr = attach_shared_segment(shmid);
  // printf("addr = %p\n", addr);
  shm = (ssegment_t *)addr;
  s_seg = (segment_meta_t *)addr;
  if (s_seg->init_status == 2) {
    // printf("debug: Acquired lock!\n");

    int index = find_empty_child_slot(s_seg->client_status);
    // printf("Index is : %d\n", index);
    if (index == -1) {
      fprintf(stderr, "Max Client Processes reached.\n");
      if (sem_post(semaphore) < 0) {
        fprintf(stderr, "Sem_post failed!\n");
      }
      // Release lock
      /* if (sem_post(semaphore) < 0) { */
      /*   fprintf(stderr, "Sem_post failed!\n"); */
      /*   return NULL; */
      /* } */
      sem_close(semaphore);
      // printf("debug: shm full released lock\n");
      return NULL;
    }

    client_index = index;
    s_seg->client_status[index] = 1;
    // printf("debug: Registered client : %d!\n", index);

    // Release lock
    if (sem_post(semaphore) < 0) {
      fprintf(stderr, "Sem_post failed!\n");
      return NULL;
    }

    // printf("debug: link - Released lock!\n");

    return &(s_seg->client_data[index]);
  }

  return NULL;
}

int
stats_unlink(key_t key) {
  // printf("Inside stats_unlink");
  // detach_shared_segment(shm);
  segment_meta_t *s_seg = (segment_meta_t *)shm;

  // Acquire lock
  if (sem_wait(semaphore) < 0) {
    fprintf(stderr, "Sem_wait failed!\n");
    return -1;
  }
  // printf("debug: Acquired lock!\n");
  int index = find_current_child_slot(s_seg->client_data);
  if (index > -1) {
    // printf("Debug release index: %d for pid: %d\n", index, getpid());
    s_seg->client_status[index] = 0;
    int det_ret = shmdt(shm);
    // Release lock
    if (sem_post(semaphore) < 0) {
      fprintf(stderr, "Sem_post failed!\n");
      return -1;
    }
    // printf("debug: Released lock!\n");
    sem_close(semaphore);

    return det_ret;
  }
  //  s_seg->client_status[client_index] = 0;
  if (sem_post(semaphore) < 0) {
      fprintf(stderr, "Sem_post failed!\n");
      return -1;
  }
  // printf("debug-error: Released lock!\n");
  sem_close(semaphore);

  return -1;
}

int find_current_child_slot(stats_t client_data[]) {
  int i;
  int pid = getpid();
  for (i = 0; i < MAX_CLIENT_COUNT; i++) {
    if (client_data[i].pid == pid) {
      return i;
    }
  }
  return -1;
}

int find_empty_child_slot(int client_status[]) {
  int i;
  for (i = 0; i < MAX_CLIENT_COUNT; i++) {
    if (client_status[i] == 0) {
      return i;
    }
  }
  return -1;
}

void init_semaphore(int key) {
  get_semaphore_name(key, sem_name);
  // Init semaphore
  semaphore = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 1);
  if (SEM_FAILED == semaphore) {
    perror("Error");
    exit(1);
  }
}
