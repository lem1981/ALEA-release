#include <unistd.h>

extern pid_t child_threads[MAX_THREADS_NUMBER];
extern int   thread_core[MAX_THREADS_NUMBER];

extern int is_thread_active( pid_t pid);
extern size_t get_child_threads( pid_t pid);
extern int threads_number;
extern int num_of_active_threads;
