#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>

#define MAX_CLIENT_COUNT 16
#define TRUE 1

typedef struct {
  int init_status;
  int client_status[MAX_CLIENT_COUNT];
  void *head;
} segment_meta_t;

typedef struct {
  int shmid;
  char *addr;
} ssegment_t;

//Util func signatures.
int parseKey(int *, char*);
long getPageSize(void);

//Stat server func signatures.
int create_shared_segment(int key);
char* attach_shared_segment(int shmid);
segment_meta_t* initialize_segment(char *addr);
int get_shared_segment(int);
void read_segment(segment_meta_t *);
void cleanup();
void init_s_handler(struct sigaction *);
