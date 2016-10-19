#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

//Util func signatures.
int parseKey(int *key, char *keys);

//Stat server func signatures.
int create_shared_segment(int key);

