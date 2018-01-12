
/*
 * mutex policy implementation
 */

int t[100];
int t_pause;

int	sys_mtxopen(struct proc *p, void *v, register_t *retval)
{
	int error = 0;
	struct sys_mtxopen_args *uap = v;
	const int KMSGMAX = 10;
	char * kmsg = malloc(KMSGMAX, M_TEMP, M_WAITOK);
	size_t done;
	error = copyinstr(SCARG(uap, name), kmsg, KMSGMAX, &done);
	printf(kmsg);
	*retval = 0;
	return error;
}

int	sys_mtxclose(struct proc *p, void *v, register_t *retval)
{
	int error = 0;
	pid_t thread_id = p->p_tid;
	printf("%d\n", thread_id);
	*retval = 0;
	return error;
}

int	sys_mtxlock(struct proc *p, void *v, register_t *retval)
{
	int error = 0;
	*retval = 0;
	return error;
}

int	sys_mtxunlock(struct proc *p, void *v, register_t *retval)
{
	int error = 0;
	*retval = 0;
	return error;
}

int	sys_mtxlist(struct proc *p, void *v, register_t *retval)
{
	int error = 0;
	*retval = 0;
	return error;
}

int	sys_mtxgrant(struct proc *p, void *v, register_t *retval)
{
	int error = 0;
	*retval = 0;
	return error;
}