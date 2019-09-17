#include "alea.h"
#include "alea_common.h"
#include "alea_threads.h"
#include "alea_ptrace.h"
#include "power_iface.h"

static void
sample_all_threads(const pid_t child_pid)
{
   int t;

   for (t = 0; t < threads_number; t++)
   {
      if ( /*(is_thread_active(child_threads[t]) == 0) ||*/  (child_threads[t] <= 0))
      {
//       child_threads[t] = 0;
         continue;
      }
      
      if ( sample_ip_register(child_threads[t], thread_core[t]) == 0)
      {
         threads_number = 1;
         break;
      }
      num_of_active_threads = t + 1;
    }
}/* sample_all_threads*/

void alea_sample( const pid_t child_pid)
{
   double power;
   /* Take power measurements */

   if ( TAKE_POWER_MEASUREMENTS )
   {
      power = alea_get_power();
   }
   else
   {
      power = 0;
   }

   LOG("Power %6f , ",power);

   /* attach to threads */
   attach_to_child_threads( child_pid);

   
   sample_all_threads( child_pid);

   LOG("\n");

   /* detach threads */
   detach_child_threads( child_pid);   
} /* alea_sample */
