#include "stats_server.h"

// sem_t *semaphore;
ssegment_t *shm;
int key;

void
usage() {
  fprintf(stderr,
  "Usage: stats_client -k key -p priority -s sleeptime_ns -c cputime_ns\n");
}

// Loop for input CPU Time using getclocktime
uint64_t
looptime(uint64_t time) {
  // init struct for getclocktime
  uint64_t diff;
  struct timespec start, end;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); /* mark start time */
  while (1) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end); /* mark the end time */
    diff = BILLION * (end.tv_sec - start.tv_sec)
            + end.tv_nsec - start.tv_nsec;
    if (diff >= time) {
      break;
    }
  }
  return diff;
}

// Convert nanosecs(unit64_t) to secs(double)
double
ns_to_s(uint64_t time_ns) {
  double time_s = time_ns*1e-9;
  return time_s;
}

// Truncate argv[0] - might not need this
void
trunc_arg(char* proc_arg) {
  int i;
  if (strlen(proc_arg) > 15) {
    if ((proc_arg[0] == '.') && (proc_arg[1] == '/')) {
      for (i = 0; i < 15; i++)  {
        proc_arg[i] = proc_arg[i+2];
        if (proc_arg[i+2] == '\0') {
          break;
        }
      }
    }
    proc_arg[15] = '\0';
    // printf("Trunc proc_name: %s\n", proc_arg);
  }
}

int main(int argc, char *argv[]) {
  // Set up SIGINT handler
  struct sigaction act;
  init_s_handler(&act);

  // Parse & extract key from command line opts
  int c, priority;
  long sleeptime_ns, cputime_ns;
  key = -1;
  priority = -21;
  sleeptime_ns = cputime_ns = -1;

  while ((c = getopt(argc, argv, "k:p:s:c:")) != -1) {
    switch (c) {
    case 'k':
      if (parseKey(&key, optarg) < 0) {
        fprintf(stderr, "Error: Invalid key %s\n", optarg);
        exit(1);
      }
      break;
    case 'p':
      if (parseKey(&priority, optarg) < 0) {
        fprintf(stderr, "Error: Invalid priority %s\n", optarg);
        exit(1);
      }
      break;
    case 's':
      if (parseLongArg(&sleeptime_ns, optarg) < 0) {
        fprintf(stderr, "Error: Invalid Sleeptime %s\n", optarg);
        exit(1);
      }
      break;
    case 'c':
      if (parseLongArg(&cputime_ns, optarg) < 0) {
        fprintf(stderr, "Error: Invalid CPU Time %s\n", optarg);
        exit(1);
      }
      break;
    default:
      usage();
      exit(1);
    }
  }

  // Make modifications for default values
  if (key == -1) {
    usage();
    exit(1);
  }

  /* if (priority == -1) { */
  /*   priority = 10; */
  /* } */
  if (sleeptime_ns == -1) {
    sleeptime_ns = 10000000L;
  }
  if (cputime_ns == -1) {
    cputime_ns = 500000000L;
  }

  // printf("CMD : %s -k %d -p %d -s %ld -c %ld\n", argv[0], key, priority,
  // sleeptime_ns, cputime_ns);

  // Initialize sleep time for nanosleep
  struct timespec sleeptime;
  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = sleeptime_ns;

  // Initialize variables for CPU Time
  uint64_t cputime = (uint64_t) cputime_ns;
  uint64_t actual_cputime;
  uint64_t cumulative_cputime = 0;
  double cumulative_cpu_secs = 0;

  // Initialize variables for Priority
  int which = PRIO_PROCESS;
  id_t pid;
  int set_prio_ret, get_prio_ret;

  // Get key from argv[1]
  // int key = atoi(argv[1]);
  // printf("key = %d\n", key);

  // Get semaphore
  // semaphore = get_semaphore(key);
  // printf("got semaphore\n");

  stats_t * c_stats = stats_init((key_t)key);
  if (NULL == c_stats) {
    fprintf(stderr, "Couldn't find segment for process\n");
    exit(1);
  }

  // Set Priority
  pid = getpid();
  if (priority > -21) {
    set_prio_ret = setpriority(which, 0, priority);
    if (set_prio_ret != 0) {
      fprintf(stderr, "Couldn't set the priority %d \n", priority);
      exit(1);
    }
  }
  // printf("Set Priority Success = %d\n", set_prio_ret);
  // printf("Child init done!\n");
  c_stats->priority = getpriority(which, pid);
  c_stats->pid = (int) getpid();
  c_stats->counter = 0;
  c_stats->cpu_secs = 0.0;

  // Check and truncate argv[0] length
  char *proc_name = argv[0];
  trunc_arg(proc_name);
  strcpy(c_stats->process_name, proc_name);

  while (TRUE) {
    // printf("Sleeping!\n");
    nanosleep(&sleeptime, NULL);


    // CPU Time
    actual_cputime = looptime(cputime);
    cumulative_cputime += actual_cputime;
    cumulative_cpu_secs = ns_to_s(cumulative_cputime);
    c_stats->cpu_secs = cumulative_cpu_secs;

    // Priority
    get_prio_ret = getpriority(which, pid);
    c_stats->priority = get_prio_ret;
    c_stats->counter++;
    /*printf("PID: %d ||Name: %s ||Cnt: %d || Prio: %d\n"
            "CumulCPUt(s): %f ||Actual CPUt: %llu\n",
            c_stats->pid, c_stats->process_name, c_stats->counter,\
            c_stats->priority, c_stats->cpu_secs,\
            (long long unsigned int) actual_cputime);

    printf("PID: %d ||Cnt: %d ||CumulCPUt(ns): %llu ||CumulCPUt(sec): %f\n",
            c_stats->pid, c_stats->counter, \
            (long long unsigned int) cumulative_cputime, \
            cumulative_cpu_secs);*/
  }

  return 0;
}

void sigint_handler(int signum) {
  // printf("in signal handler\n");
  stats_unlink((key_t) key);
  // printf("unlink_ret : %d\n", unlink_ret);
  _exit(0);
}

void init_s_handler(struct sigaction *act) {
  memset (act, '\0', sizeof(*act));
  act->sa_handler = sigint_handler;
  sigaction(SIGINT, act, NULL);
}
