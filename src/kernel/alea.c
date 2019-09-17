#include "alea.h"
#include "alea_common.h"
#include "alea_threads.h"
#include "alea_arch_iface.h"

#include <sys/types.h>
#include <unistd.h>

/**
 * Size of sampling period
 */
unsigned long alea_smpl_period = 0;
unsigned long alea_smpl_period_sec = 0;
unsigned long alea_first_random_delay_sec = 0;
unsigned long alea_first_random_delay = 0;
unsigned long alea_sys_delay_sec = 0;
unsigned long alea_sys_delay = 0;

/**
 * The basic settings of profiling
 */
int alea_collect_ip_stat = 0;
int alea_collect_ip_exec_stat = 0;
int alea_specific_cpu = -1;
int alea_cpu_num = -1;
int alea_core_num = 8;

static
void alea_constructor()
{
   alea_init_output();
//   power_meters_constructor();
}/*  alea_constructor */

static
void alea_destructor()
{
   if ( TAKE_POWER_MEASUREMENTS)
   {
      power_meters_destructor();
   }
   alea_close_output();
}/*  alea_constructor */


/**
 * Profiling: the main loop 
 */
static void 
alea_profile(int argc, char *argv[])
{
   pid_t child_pid;
   pid_t waitpid_res = 0;
   pid_t status = 0;

   /* Set ALEA options */
   alea_set_options();

   /* Create a thread to run a profiled applitcation */
   child_pid = fork();

   if ( child_pid == 0)
   {
       /* Run a profiled application */
       execve(argv[1], &(argv[1]), environ);
       ERROR("ERROR after execve!!!!!:\n");
   }
   else if( child_pid > 0 ) //pid>0, parent, waitfor child
   {

       start_program_timer();

       /**
        * First delay is required for initialization of a profiled application
        * Execve needs some time to start an application. Segmentation fault is possible if
        * ALEA interrupts the application before it is started by execve
        */
       alea_delay( alea_sys_delay_sec, alea_sys_delay);

       /**
        * First sample is taken randomly in systematic sampling
        */
       alea_delay( alea_first_random_delay_sec, alea_first_random_delay);

       DEBUG("Start sampling...");

       while ( 1 )
       {
          /**
           * Get status of the child thread 
           */
          waitpid_res = waitpid(child_pid, &status, WNOHANG);


          DEBUG("Take a sample...");
          DEBUG("Profiled app status %d\n", (int)waitpid_res);

          if ( ( waitpid_res != 0) || (is_thread_active(child_pid) == 0) )
          {
             LOG("Program time %.6f\n", get_program_time());
             break;
          }

          /**
           * Sampling
           */
          alea_sample(child_pid); 
          
          alea_delay(alea_smpl_period_sec, alea_smpl_period);
       }
   }
}/* alea_profile */

int main(int argc, char *argv[])
{
   alea_constructor();

   alea_profile(argc, argv);

   alea_destructor();
}/* main */
