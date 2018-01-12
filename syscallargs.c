/*
 * System call argument lists.
 */

struct sys_mtxopen_args {
	syscallarg(const char *) name;
};

struct sys_mtxclose_args {
	syscallarg(int) d;
};

struct sys_mtxlock_args {
	syscallarg(int) d;
};

struct sys_mtxunlock_args {
	syscallarg(int) d;
};

struct sys_mtxlist_args {
	syscallarg(int) d;
	syscallarg(pid_t *) pidlist;
	syscallarg(size_t) nlist;
};

struct sys_mtxgrant_args {
	syscallarg(int) d;
	syscallarg(pid_t) selected;
};

/*
 * System call prototypes.
 */

int	sys_mtxopen(struct proc *, void *, register_t *);
int	sys_mtxclose(struct proc *, void *, register_t *);
int	sys_mtxlock(struct proc *, void *, register_t *);
int	sys_mtxunlock(struct proc *, void *, register_t *);
int	sys_mtxlist(struct proc *, void *, register_t *);
int	sys_mtxgrant(struct proc *, void *, register_t *);