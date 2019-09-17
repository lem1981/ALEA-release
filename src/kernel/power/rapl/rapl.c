#include "alea.h"
#include "alea_common.h"
#include "rapl.h"

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>

static int       msr[ALEA_MAX_CPU_NUM];
static double    energy_units = 0 ;
static long long pp0_prev_cnt = 0;
static long long pkg_prev_cnt = 0;
static long long dram_prev_cnt = 0;
static double    pp0_sum = 0;
static double    pkg_sum = 0;
static double    dram_sum = 0;

static
long long read_msr(int msr_cpu, int which);

static
int open_enrg_driver(int core)
{
  char filename[ALEA_MAX_STR_LEN];
  int  msr_cpu;

  sprintf(filename, MSR_FILE, core);

  msr_cpu = open(filename, O_RDONLY);
  if ( msr_cpu < 0 )
  {
    if ( errno == ENXIO )
    {
      ERROR("rdmsr: Error\n");
    } else if ( errno == EIO ) 
    {
      ERROR("rdmsr: CPU doesn't support MSRs\n");
    } 
    else 
    {
      ERROR("rdmsr:Can't open MSR register \n");
    }
  }

  return msr_cpu;
}/* open_enrg_driver */

static
long long read_energy_counter( int counter_type)
{
   long long result = 0;
   int       i;


   if ( msr[0] == 0 )
   {
     DEBUG("Open MSR drivers\n");

     if ( alea_cpu_num == 1)
     {
        msr[0] = open_enrg_driver( alea_specific_cpu);
        result = read_msr( msr[0], MSR_RAPL_POWER_UNIT);
        energy_units = pow(0.5,(double)((result>>8)&0x1f));
     }
     else
     {
        for( i=0; i < alea_cpu_num; i++)
        {
           msr[i] = open_enrg_driver( i*alea_core_num);
           result = read_msr( msr[i], MSR_RAPL_POWER_UNIT);
           energy_units = pow(0.5,(double)((result>>8)&0x1f));
        }
     }
   }

   result = 0;

   for( i=0; i < alea_cpu_num; i++)
   {
      result=result+read_msr( msr[i], counter_type);
      DEBUG("Reading msr driver %d, value %ld\n",i,result);
   }

   return result;
}/* read_energy_counter */

static
long long read_msr(int msr_cpu, int which)
{

  uint64_t data;

  if ( pread(msr_cpu, &data, sizeof data, which) != sizeof data )
  {
    ERROR("rdmsr:pread");
  }

  return (long long)data;
}/* read_msr */

static
void close_drivers()
{
   int i;

   if ((msr[0] != 0))
   {
      for( i=0; i < alea_cpu_num; i++)
      {
         close(msr[i]);
      }
   }
}/* close_drivers */

static
double rapl_read_pp0_energy_cnt(int counter_type)
{
  long long pp0_cur_cnt;
  double pp0_energy;

  pp0_cur_cnt = read_energy_counter(counter_type);

  DEBUG("Total PP0 energy %f \n", ((double)pp0_cur_cnt)*energy_units);

  if ( pp0_cur_cnt >= pp0_prev_cnt)
  {
     pp0_energy = (double)( pp0_cur_cnt - pp0_prev_cnt)*energy_units;
  }
  else
  {
     pp0_energy = (double) (MSR_RAPL_MAX_CNT - pp0_prev_cnt + pp0_cur_cnt)*energy_units;
  }

  pp0_prev_cnt = pp0_cur_cnt;

  return (pp0_energy);
}/* rapl_read_pp0_energy_cnt() */


double alea_get_power()
{
   struct timespec start_sample_time, stop_sample_time;
   struct timespec tw;
   struct timespec tr;
   double energy, time;

   /* Set power averaging period */
   tw.tv_sec = 0;
   tw.tv_nsec = RAPL_DEF_SAMPLE_POWER_PERIOD;

   clock_gettime(CLOCK_MONOTONIC, &start_sample_time);

   rapl_read_pp0_energy_cnt(MSR_PP0_ENERGY_STATUS);
   nanosleep(&tw,&tr);
   energy = rapl_read_pp0_energy_cnt(MSR_PP0_ENERGY_STATUS);

   DEBUG("Sampled energy %f\n", energy);

   clock_gettime(CLOCK_MONOTONIC, &stop_sample_time);

   time = timediff(start_sample_time,stop_sample_time);

   DEBUG("Power averaging period %f\n", time);

   return (energy/time);
}/* alea_get_power */

void power_meters_destructor()
{
   close_drivers();
}/* power_destructor */
