#include "alea.h"
#include "alea_common.h"
#include "alea_threads.h"
#include "alea_ptrace.h"
#include "power_iface.h"

#include <errno.h>
#include <sys/ptrace.h>
#include <sys/user.h>

extern int sample_ip_register( pid_t tid, pid_t core);
