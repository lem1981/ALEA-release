#include "alea_common.h"
#include "time.h"

static struct timespec start_program_time, stop_program_time;

FILE *debug_file;
FILE *stat_file;

double timediff(struct timespec start, struct timespec end)
{
   long sec;
   long nsec;

   if ( (end.tv_nsec-start.tv_nsec ) <0 ) 
   {
      sec = end.tv_sec-start.tv_sec-1;
      nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
   } 
   else 
   {
      sec = end.tv_sec-start.tv_sec;
      nsec = end.tv_nsec-start.tv_nsec;
   }

   return (double)((double)sec+((double)nsec/(double)1000000000.0));
}

/**
 * First system delay.
 * First delay is required to create environment for
 * a profiled application
 */
void alea_delay( unsigned long sec, unsigned long msec )
{
   struct timespec tw;
   struct timespec tr;

   tw.tv_sec = sec;
   tw.tv_nsec = msec;

   SLEEP(tw,tr);
}/* alea_delay */

void start_program_timer()
{
   clock_gettime(CLOCK_MONOTONIC, &start_program_time);
}/* start_program */

float get_program_time()
{
   clock_gettime(CLOCK_MONOTONIC, &stop_program_time);
   return (timediff(start_program_time,stop_program_time));
}/* start_program */

void
alea_init_output()
{
   if ((stat_file = fopen("energy.prof", "w+")) == NULL)
   {
      ERROR("Can't open a file to dump statistic\n");
   }

   #ifdef DEBUG_OUTPUT
   if ((debug_file = fopen("debug.prof", "w+")) == NULL)
   {
      ERROR("Can't open a file to dump debug info\n");
   }
   #endif /* DEBUG */
}/* alea_init_output */

void
alea_close_output()
{
   fclose(stat_file);
   #ifdef DEBUG_OUTPUT
   fclose(debug_file);
   #endif /* DEBUG */
}/* alea_init_output */
