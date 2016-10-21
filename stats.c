#include "stats.h"
#include "stat_server.h"

stats_t* stat_init(key_t key);
int stat_unlink(key_t key);
int find_empty_child_slot(int client_status[]);

stats_t* stat_int(key_t key) {
  int shmid = get_shared_segment(key);
  char *addr = attach_shared_segment(shmid);

  segment_meta_t *s_seg = (segment_meta_t *)addr;
  if (s_seg->init_status == 2) {
    int index = find_empty_child_slot(s_seg->client_status);
    if (index == -1) {
      fprintf(stderr, "Max Client Processes reached.\n");
      return NULL;
    }

    s_seg->client_status[index] = 1;
    return s_seg->head + index*sizeof(stats_t);
  }

  return NULL;
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
