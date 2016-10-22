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
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "stats.h"

#define MAX_CLIENT_COUNT 16
#define TRUE 1
#define MAX_KEY_LENGTH 1024

typedef struct {
  int init_status;
  int client_status[MAX_CLIENT_COUNT];
  // char *head;
  // stats_t *head;
  stats_t client_data[MAX_CLIENT_COUNT];
} segment_meta_t;

typedef struct {
  int shmid;
  char *addr;
} ssegment_t;

// Func declarations
void sigint_handler(int signum);
void init_s_handler(struct sigaction *act);
void cleanup();
void sigint_handler(int signum);

//Util func signatures.
int parseKey(int *, char*);
long getPageSize(void);
void get_semaphore_name(int key, char *sem_name);

//Stat server func signatures.
int create_shared_segment(int key);
char* attach_shared_segment(int shmid);
segment_meta_t* initialize_segment(char *addr);
int get_shared_segment(int);
void read_segment(segment_meta_t *);
sem_t* init_semaphore(int key);
sem_t* get_semaphore(int key);

//stats.c
stats_t* stat_init(key_t key);
int stat_unlink(key_t key);
int find_empty_child_slot(int client_status[]);
