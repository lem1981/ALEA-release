#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "time.h"

extern void alea_delay( unsigned long sec, unsigned long msec );
extern void start_program_timer();
extern float get_program_time();
extern double timediff(struct timespec start, struct timespec end);

extern FILE *debug_file;
extern FILE *stat_file;

/**
 * Debug output
 */
#ifdef DEBUG_OUTPUT
  #define DEBUG(...) fprintf(debug_file, __VA_ARGS__);\
                     fflush(debug_file)
#else
  #define DEBUG(...)
#endif /* DEBUG */

#define LOG(...) fprintf(stat_file, __VA_ARGS__);
#define ERROR(...) perror(__VA_ARGS__);\
                   exit(1)


#define SLEEP(tw,tr) nanosleep(&tw,&tr)

#define ALEA_MAX_STR_LEN 256
