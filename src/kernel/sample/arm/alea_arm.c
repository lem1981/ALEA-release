#include "alea_arm.h"

int 
sample_ip_register( pid_t tid, pid_t core)
{
   struct pt_regs regs;
   long   r;
   long   try_num = 0;
   long   max_try_num = ALEA_MAX_PTRACE_TRY_NUM;

   DEBUG("Trying to get regs for thread %d, core %d\n", (int)tid,(int)core);
   do
   {
      r = ptrace(PTRACE_GETREGS, tid, NULL, &regs);
      try_num++;
   } while ((r == -1L) && (errno == ESRCH) && (try_num < max_try_num));

   DEBUG("Tries num %ld, for thread %d, core %d\n",try_num, (int)tid,(int)core);
   if ((r == -1L) || ( try_num >= max_try_num))
   {
      DEBUG("Exit.the number of tries was exceed\n");
      return 0;
   }

   LOG("Thread %d, core %d: RIP=0x%x ", (int)tid,(int)core, (unsigned int)regs.ARM_pc);

   return 1;
}/* sample_ip_register  */
