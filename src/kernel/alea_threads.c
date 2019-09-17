#include "alea.h"
#include "alea_threads.h"
#include "alea_common.h"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/syscall.h>

pid_t child_threads[MAX_THREADS_NUMBER];
int   thread_core[MAX_THREADS_NUMBER];
int   threads_number;
int   num_of_active_threads;

/**
 * Local function prototypes
 */
static inline
int get_thread_core( pid_t pid, pid_t thread_id);

/**
 * Check the status of the thread/process
 */
int is_thread_active( pid_t pid)
{
   char  stat_file_name[ALEA_MAX_STR_LEN];
   char  stat_info[ALEA_MAX_STR_LEN];
   char  *str_ptr;
   FILE  *file_stat;
   int   j;

   if (snprintf(stat_file_name, sizeof stat_file_name, "/proc/%d/stat",
                              (int)pid) >= (int)sizeof stat_file_name)
   {
      errno = ENOTSUP;
      return (0);
   }

   DEBUG("Trying to open file %s\n", stat_file_name);

   file_stat = fopen(stat_file_name, "r");
   if (!file_stat)
   {
      errno = ESRCH;
      return (0);
   }

   DEBUG("Read status string for the thread %d from file %s\n", pid, stat_file_name);
   if ( fgets( stat_info, ALEA_MAX_STR_LEN, file_stat) == NULL)
   {

      DEBUG("Can't read file %s\n", stat_file_name);
      fclose(file_stat);
      return (0);
   }

   /**
    * Parse the status string 
    */
   str_ptr = stat_info;
   j=0;
   while ( j < 2)
   {
      str_ptr = strchr(str_ptr,' ');
      str_ptr++;
      j++;
   }

   fclose(file_stat);

   DEBUG("Status %c\n", *str_ptr);

   if ( (*str_ptr != 'Z') && (*str_ptr != 'X'))
   {
      return (1);
   }

   return (0);
} /* is_thread_active */

/**
 * Identify tids for all child threads of the process
 * with id - pid.
 *
 * Return value: number of child threads
 */
size_t get_child_threads( pid_t pid)
{
    char     dirname[ALEA_MAX_STR_LEN];
    DIR     *dir;
    size_t   used = 0;
    size_t   i=0;
    struct   dirent *ent;
    int      thread_id;
    char     dummy;

#if 0
    if ( threads_number > 1)
    {
       /* 
        * Optimization:
        * if threads_number > 1 then we've already
        * got information about child threads.
        * So, we can pass searching of child threads
        * procedure. Is some of child threads do not exist
        * then we get an error during attaching to thread
        * process. In this case we wee run the procedure
        * of child threads searching
        */
       return ( threads_number);
    }
#endif
    if ( pid < (pid_t)1)
    {
        return (size_t)0;
    }

    if ( is_thread_active(pid) == 0)
    {
       return (size_t)0;
    }

   DEBUG("Form path to open dir for pid %d\n", pid);

   /**
    * Get information about child threads of pid through
    * directory /proc/<pid>/task
    */
   if ( snprintf(dirname, sizeof dirname, "/proc/%d/task/", 
                          (int)pid) >= (int)sizeof dirname)
   {
       return (size_t)0;
   }

   DEBUG("Trying to open dir for pid %d...\n", pid);

   /**
    * Open the directory
    */
   dir = opendir(dirname);
   if (!dir)
   {
       return (size_t)0;
   }

   DEBUG("Opened the dir for pid %d...\n", pid);

   /**
    * In directory /proc/<pid>/task should be located
    * all child threads of process pid
    * 
    * Fetch all child threads id from the directory
    */
   while ( used < MAX_THREADS_NUMBER)
   {
      if ( is_thread_active(pid) == 0)
      {
         return (size_t)0;
      }

      DEBUG("Trying to read ent \n");

      /* reading the directory with child threads id*/
      errno = 0;
      ent = readdir(dir);

      if (!ent)
         break;

      DEBUG("Trying to parse thread id \n");

      /* Parse child thread id */
      if (sscanf(ent->d_name, "%d%c", &thread_id, &dummy) != 1)
         continue;

      DEBUG("Parsed \n");

      /* Ignore obviously invalid ids */
      if (thread_id < 1)
          continue;

      DEBUG("Adding thread id to the list \n");

      if ( is_thread_active(thread_id))
      {
         child_threads[used++] = (pid_t)thread_id;
      }
  }

  for( i = used; i  < MAX_THREADS_NUMBER; i++)
  {
     child_threads[i] = (pid_t)0;
  }

  /**
   * Get core id for each thread
   */
  for ( i = 0; i < used; i++)
  {
     if ( is_thread_active(child_threads[i]))
     {
        thread_core[i] = get_thread_core( pid, child_threads[i]);
     }
  }

  if (errno)
  {
     const int saved_errno = errno;
     closedir(dir);
     errno = saved_errno;
     return (size_t)0;
  }

  if (closedir(dir))
  {
     errno = EIO;
     return (size_t)0;
  }

  if ( !used)
  {
     DEBUG("Parent: Could not find child threads \n");
  }

  DEBUG("Process %d with thread id %d has %d tasks,\n", 
                 (int)pid, (int)syscall(SYS_gettid),(int)threads_number);

  return (used);
} /*get_child_threads */

/**
 * Get thread core id
 */
static inline
int get_thread_core( pid_t pid, pid_t thread_id)
{
   char  stat_file_name[ALEA_MAX_STR_LEN];
   char  stat_info[ALEA_MAX_STR_LEN];
   char  *str_ptr;
   FILE  *file_stat;
   int   j, core_id;

   if (snprintf(stat_file_name, sizeof stat_file_name, "/proc/%d/task/%d/stat",
                              (int)pid, (int)thread_id) >= (int)sizeof stat_file_name)
   {
      errno = ENOTSUP;
      return (size_t)0;
   }

   DEBUG("Trying to open file %s\n", stat_file_name);

   file_stat = fopen(stat_file_name, "r");
   if (!file_stat)
   {
      errno = ESRCH;
      return (size_t)0;
   }

   DEBUG("Trying to read a string from  file %s\n", stat_file_name);

   if ( fgets( stat_info, ALEA_MAX_STR_LEN, file_stat) == NULL)
   {
      DEBUG("Can't read file %s\n", stat_file_name);
      fclose(file_stat);
      return 0;
   }

   j=0;
   while ( j < 6)
   {
      str_ptr = strrchr(stat_info,' ');
      *str_ptr = '\0';
      j++;
   }
   core_id = atoi(str_ptr+1);

   fclose(file_stat);

   return (core_id);
} /* get_thread_core */
