#include "stat_server.h"

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
