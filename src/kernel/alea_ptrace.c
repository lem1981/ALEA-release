#include <errno.h>
#include <sys/ptrace.h>
#include "alea.h"
#include "alea_common.h"
#include "alea_threads.h"


/**
 * Attach to child threads to get
 * information about currect IP.
 */
void attach_to_child_threads( const pid_t pid)
{
  size_t t;
  long   r;

  DEBUG("Attach threads...\n");

  threads_number = get_child_threads( pid);


  DEBUG("Threads number...%d\n", threads_number);

  for (t = 0; t < threads_number; t++)
  {
     do
     {
        if ( is_thread_active(child_threads[t]))
        {
           r = ptrace(PTRACE_ATTACH, child_threads[t], (void *)0, (void *)0);
        }
        else
        {
           child_threads[t]=0;
           break;
        }
     } while (r == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));

     /* ERROR! during attach process - detach */
     if (r == -1L)
     {

        DEBUG("Error during attaching.Detach\n");

        while (t-->0)
        {
           do
           {
              if ( (child_threads[t] > 0) &&  is_thread_active(child_threads[t]))
              {
                 r = ptrace(PTRACE_DETACH, child_threads[t], (void *)0, (void *)0);
              }
              else
              {
                 break;
              }
           } while (r == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
        }

        threads_number = 0;

        DEBUG("Error during attaching.Detached\n");

        break;
     }

     DEBUG("Attached\n");
   }
} /* attach_to_child_threads */

/**
 * Detach from child threads
 */
void detach_child_threads( const pid_t pid)
{
   int i;
   long r;

   DEBUG("Detach threads...\n");

   for (i = 0; i < num_of_active_threads; i++)
   {
      do
      {
         if ( (child_threads[i] > 0) &&  is_thread_active(child_threads[i]))
         {
            r = ptrace(PTRACE_DETACH, child_threads[i], (void *)0, (void *)0);
         }
         else
         {
            break;
         }
      } while (r == -1 && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
   }

   DEBUG("Detached\n");
}/* detach_child_threads */
