#include "alea.h"
#include "alea_common.h"
#include <sys/time.h>

/**
 * Module : parse_options.c 
 *
 * This module is used to parse options set
 * by evnironmental variable
 */

/**
 * Parse environmental ovariables to
 * set  ALEA options
 */
static void parse_options()
{
   char* tmp_str;

   alea_specific_cpu = ALEA_DEFAULT_SPECIFIC_CPU;
   alea_cpu_num = ALEA_DEFAULT_CPU_NUM;
   alea_core_num = ALEA_DEFAULT_CORE_NUM;

   DEBUG("Parse options\n");

   /**
    * Take power and energy measuements for
    * specific CPU
    */
   tmp_str = getenv("ALEA_SPECIFIC_CPU");
   if ( tmp_str != NULL)
   {
      alea_specific_cpu = atof( tmp_str);
   }
   else
   {

      /**
       * Take measurements for several CPUs
       * specific CPU
       */
      tmp_str = getenv("ALEA_CPU_NUM");

      if ( tmp_str != NULL)
      {
         alea_cpu_num = atof( tmp_str);

        /**
         * We limit the maximum number of CPUs
         */
         if (alea_cpu_num > ALEA_MAX_CPU_NUM)
         {
            alea_cpu_num = ALEA_MAX_CPU_NUM;
         }


        /**
         * Take measurements for several CPUs
         * specific CPU
         */
         tmp_str = getenv("ALEA_CORE_NUM");
         if ( tmp_str != NULL)
         {
            alea_core_num = atof( tmp_str);

           /**
            * We limit the maximum number of cores
            */
            if (alea_core_num > ALEA_MAX_CORE_NUM)
            {
               alea_core_num = ALEA_MAX_CORE_NUM;
            }
         }
      }
   }

  /**
   * Take energy measurements without profiling
   * for entire application
   */
   alea_collect_ip_stat = 1;
   tmp_str = getenv("ALEA_MEASURE_ENERGY");
   if ( tmp_str != NULL)
   {
      alea_collect_ip_stat = 0;
   }

  /**
   * Execution time profiling without energy profiling
   */
   alea_collect_ip_exec_stat = 0;
   tmp_str = getenv("ALEA_COLLECT_ONLY_IP_STAT");
   if ( tmp_str != NULL)
   {
      alea_collect_ip_exec_stat = 1;
   }

   /**
    * Set size of sampling period in milliseconds
    */
   tmp_str = getenv("ALEA_SAMPLING_PERIOD");

   if ( tmp_str == NULL)
   {
     if ( alea_collect_ip_stat)
     {
        /* Set sampling period for profiling mode */
        alea_smpl_period = ALEA_SAMPLING_PERIOD;
     }
     else
     {
        /* Set sampling period for measurement mode */
        alea_smpl_period = ALEA_DEF_MEASURE_PERIOD;
     }
   }
   else
   {
      alea_smpl_period = atol( tmp_str);
   }

   /**
    * Set size of sampling period in seconds
    */
   tmp_str = getenv("ALEA_SAMPLING_PERIOD_SEC");

   if ( tmp_str == NULL)
   {
     if ( alea_collect_ip_stat)
     {
        /* Set default sampling period for profiling mode */
        alea_smpl_period_sec = ALEA_SAMPLING_PERIOD_SEC;
     }
     else
     {
        /* Set default sampling period for measurement mode */
        alea_smpl_period_sec = ALEA_DEF_MEASURE_PERIOD_SEC;
     }
   }
   else
   {
      alea_smpl_period_sec = atol( tmp_str);
   }
} /* parse_options */


/**
 * Set first delay. ALEA uses systematic sampling, which
 * implies that the first sample is taken absolutely
 * randomly 
 */
static void set_first_delay()
{
   unsigned int srand_seed;
   struct timeval cur_tv;
   struct timezone cur_tz;


   gettimeofday( &cur_tv,  &cur_tz);

   srand((unsigned int )cur_tv.tv_usec);
   srand(srand_seed);
   srand_seed = rand();
   srand(srand_seed);

   DEBUG("srand seed %d\n", srand_seed);

   alea_first_random_delay_sec = (unsigned long)(((double)rand()*(double)alea_smpl_period_sec)/(double)RAND_MAX);
   alea_first_random_delay = (unsigned long)(((double)rand()*(double)alea_smpl_period)/(double)RAND_MAX);

   DEBUG("alea_first_random_delay_sec %d\n", alea_first_random_delay_sec);
   DEBUG("alea_first_random_delay %d\n", alea_first_random_delay);
}/* set_first_delay */

/**
 * First system delay.
 * First delay is required to create environment in 
 * a profiled application. 
 * 
 * Notes: If ALEA starts to profile before a profiled application 
 * finishes the initialization part then the application could
 * fail
 */
static
inline void set_system_delay()
{
   alea_sys_delay_sec = 0;
   alea_sys_delay = ALEA_DEF_FIRST_SYS_DELAY;
}/* first_system_delay */

/**
* Set ALEA option
*/
void alea_set_options()
{  
   DEBUG("ALEA settings\n");

   /* Parse environmental settings*/
   parse_options();

   set_system_delay();
   /**
    * Set first sampling prediod randomly according
    * to the systematic sampling
    */
   set_first_delay();
  
}/* alea_set_options */
