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

//Util func signatures.
int parseKey(int *, char*);

//Stat server func signatures.
int create_shared_segment(int key);
char* attach_shared_segment(int shmid);
void initialize_segment(char *addr);
void read_segment(char *addr);
