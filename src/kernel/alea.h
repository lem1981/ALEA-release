/**
 * Module : parse_options.h
 *
 * This module contains the global variables and description of
 * the main interfaces of ALEA
 */

/**
 * Architecture configuration
 */
#define ALEA_DEFAULT_SPECIFIC_CPU 0
#define ALEA_DEFAULT_CPU_NUM      1
#define ALEA_DEFAULT_CORE_NUM     8
#define ALEA_MAX_CPU_NUM          8
#define ALEA_MAX_CORE_NUM         12

/**
 * Default sampling period 
 */
#define ALEA_SAMPLING_PERIOD       10000000
#define ALEA_SAMPLING_PERIOD_SEC   0
#define ALEA_DEF_FIRST_SYS_DELAY   400000

#define ALEA_DEF_MEASURE_PERIOD     500000000 
#define ALEA_DEF_MEASURE_PERIOD_SEC 0
/**
* Default settings
*/
#define MAX_THREADS_NUMBER 100

#define TAKE_POWER_MEASUREMENTS (!alea_collect_ip_exec_stat)
/**
 * Functions and macros
 */
extern void alea_set_options();

/**
 * Size of sampling period
 */
extern unsigned long alea_smpl_period;
extern unsigned long alea_smpl_period_sec;
extern unsigned long alea_first_random_delay_sec;
extern unsigned long alea_first_random_delay;
extern unsigned long alea_sys_delay;
extern unsigned long alea_sys_delay_sec;


/**
 * The basic settings of profiling
 */
extern int           alea_collect_ip_stat;
extern int           alea_collect_ip_exec_stat;
extern int           alea_specific_cpu;
extern int           alea_cpu_num;
extern int           alea_core_num;
