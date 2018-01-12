#ifndef MUTEX_POLICY_H
#define MUTEX_POLICY_H

// Opens/Creates a new mutex and returns a descriptor.
// Sets errno and returns -1 in case of failure.
int sys_mtxopen(const char *name);

// Closes an open mutex for the calling process.
// Returns 0 on success, or sets errno and returns -1 in case of failure.
int sys_mtxclose(int d);

// Locks the mutex, blocking the thread if it is not available.
// Returns 0 on success, or sets errno and returns -1 in case of failure.
int sys_mtxlock(int d);

// Unlocks the mutex, releasing it from lock by any other thread.
// Returns 0 on success, or sets errno and returns -1 in case of failure.
int sys_mtxunlock(int d);

// Returns n, where the first n spots in the list have been populated with threads awaiting specified mutex.
// If n is -1, an error was encountered.l
// If n is nlist, there are probably more thread ids that could not be placed in the list. Try to repeat the call with a bigger list.
// pidlist must be an array of length at least nlist.
int sys_mtxlist(int d, pid_t *pidlist, size_t nlist);

// Grants lock of mutex to thread represented by selected.
// Returns 0 on success and sets errno and returns -1 on failure.
int sys_mtxgrant(int d, pid_t selected);

#endif 
