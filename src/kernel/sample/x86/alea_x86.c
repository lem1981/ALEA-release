#include "alea_x86.h"

int 
sample_ip_register( pid_t tid, pid_t core)
{
   struct user_regs_struct regs;
   long   r;
   long   ptrace_try_num = 0;
   long   thread_active_checks = 0;

   DEBUG("Trying to get regs for thread %d, core %d\n", (int)tid,(int)core);

   do
   {
      r = ptrace(PTRACE_GETREGS, tid, &regs, &regs);
      ptrace_try_num++;
      if ( ptrace_try_num >= ALEA_MAX_PTRACE_TRY_NUM)
      {
         ptrace_try_num = 0;
         thread_active_checks++;
         if ( is_thread_active( tid) == 0)
         {
            break;
         }
      }
   } while ((r == -1L) && (errno == ESRCH));

   DEBUG("Tries num %ld, for thread %d, core %d\n",
          ptrace_try_num+thread_active_checks*ALEA_MAX_PTRACE_TRY_NUM, (int)tid,(int)core);

   if ((r == -1L))
   {
      DEBUG("Exit.the number of tries was exceed\n");
      return 0;
   }

   #if __x86_64__
   LOG("Thread %d, core %d: RIP=0x%x ", (int)tid,(int)core, (int)regs.rip);
   #else
   LOG("Thread %d, core %d: RIP=0x%x ", (int)tid,(int)core, (int)regs.eip);
   #endif

   return 1;
}/* sample_ip_register  */
