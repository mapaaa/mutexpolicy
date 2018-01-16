#include <sys/queue.h>

struct myprocess {
	LIST_ENTRY(myprocess) link; 
	pid_t id;
	int waiting;
};

struct mymutex {
	LIST_ENTRY(mymutex) link;
	int d;
	char *name;
	LIST_HEAD(, myprocess) plist;
	int is_locked;
};

int t_pause;
pid_t wake_pid;
LIST_HEAD(, mymutex) mlist;
int mlist_is_init;

int	sys_mtxopen(struct proc *p, void *v, register_t *retval)
{
	if(mlist_is_init == 0) {
		LIST_INIT(&mlist);
		mlist_is_init = 1;
	}
	struct sys_mtxopen_args *uap = v;
	const int KMSGMAX = 10;
	
	char * kname = malloc(KMSGMAX, M_TEMP, M_WAITOK);
	size_t done;
	copyinstr(SCARG(uap, name), kname, KMSGMAX, &done);

	struct myprocess *pitem = malloc(sizeof(struct myprocess), M_TEMP, M_WAITOK);
	pitem->id = p->p_tid;
	pitem->waiting = 0;

	struct mymutex *found = NULL;
	int maxdescriptor = 0;

	struct mymutex *it;
	LIST_FOREACH(it, &mlist, link) {
		if(it->d > maxdescriptor) {
			maxdescriptor = it->d;
		}
		if(strcmp(it->name, kname) == 0) {
			found = it;
		}
	}

	if(found != NULL) {
		printf("CREAT: found mutex name %s, already openend as %d\n", kname, found->d);
		LIST_INSERT_HEAD(&(found->plist), pitem, link);
		*retval = found->d;
		return 0;
	}

	struct mymutex *item = malloc(sizeof(struct mymutex), M_TEMP, M_WAITOK);
	item->d = maxdescriptor + 1;
	item->name = malloc(KMSGMAX, M_TEMP, M_WAITOK);
	kcopy(kname, item->name, KMSGMAX);
	item->is_locked = 0;

	printf("CREAT: not found, created new mutex name %s, descriptor %d\n", kname, item->d);
	LIST_INSERT_HEAD(&mlist, item, link);

	LIST_INIT(&(item->plist));
	LIST_INSERT_HEAD(&(item->plist), pitem, link);

	*retval = item->d;
	return 0;
}

int	sys_mtxclose(struct proc *p, void *v, register_t *retval)
{
	if(mlist_is_init == 0) {
		LIST_INIT(&mlist);
		mlist_is_init = 1;
	}
	pid_t thread_id = p->p_tid;
	struct sys_mtxclose_args *uap = v;
	int d = SCARG(uap, d);

	struct mymutex *varmutex;
	LIST_FOREACH(varmutex, &mlist, link) {
		if (varmutex->d == d) {
			struct myprocess *varprocess;
			LIST_FOREACH(varprocess, &(varmutex->plist), link) {
				if (varprocess->id == thread_id) {
					LIST_REMOVE(varprocess, link);
					break;
				}
			}
			if (LIST_EMPTY((&varmutex->plist))) {
				LIST_REMOVE(varmutex, link);
			}
			break;
		}
	}
	*retval = 0;
	return 0;
}

int	sys_mtxlock(struct proc *p, void *v, register_t *retval)
{
	int error = 0;
	if(mlist_is_init == 0) {
		LIST_INIT(&mlist);
		mlist_is_init = 1;
	}
	struct sys_mtxlock_args *uap = v;
	int d = SCARG(uap, d);

	struct mymutex *ifound = NULL;
	struct mymutex *it;
	LIST_FOREACH(it, &mlist, link) {
		if (it->d == d) {
			ifound = it;
			break;
		}
	}

	if(ifound == NULL) {
		printf("LOCK: no such descriptor %d\n", d);
		*retval = -1;
		return 0;
	}

	printf("LOCK: mutex to be locked name %s, descriptor %d\n", ifound->name, ifound->d);

	struct myprocess *jfound = NULL;
	struct myprocess *jt;
	LIST_FOREACH(jt, &(ifound->plist), link) {
		if (jt->id == p->p_tid) {
			jfound = jt;
			break;
		}
	}

	if(jfound == NULL) {
		printf("LOCK: proc %d has not opened mutex name %s\n", d, ifound->name);
		*retval = -1;
		return 0;
	}

	printf("LOCK: proc %d start waiting mutex %s\n", jfound->id, ifound->name);

	jfound->waiting = 1;

	// should check mutex not in use (is_locked == 0) to skip sleep

	sleep:
	error = tsleep(&t_pause, PSOCK | PCATCH, "awaiting lock", 0);

	if(error) {
		jfound->waiting = 0;
		*retval = -1;
		return error;
	}

	if(wake_pid != p->p_tid) {
		goto sleep;
	}

	wake_pid = 0;
	jfound->waiting = 0;

	*retval = 0;
	return 0;
}

int	sys_mtxunlock(struct proc *p, void *v, register_t *retval)
{
	if(mlist_is_init == 0) {
		LIST_INIT(&mlist);
		mlist_is_init = 1;
	}
	struct sys_mtxclose_args *uap = v;
	int d = SCARG(uap, d);

	struct mymutex *varmutex;
    LIST_FOREACH(varmutex, &mlist, link) {
		if (varmutex->d == d) {
			// should check mutex d actually opened by proc p?
			varmutex->is_locked = 0;
			break;
		}
	}

	*retval = 0;
	return 0;
}

int	sys_mtxlist(struct proc *p, void *v, register_t *retval)
{
	if(mlist_is_init == 0) {
		LIST_INIT(&mlist);
		mlist_is_init = 1;
	}
	struct sys_mtxlist_args *uap = v;

	int *mtx_info =  SCARG(uap, d);
	pid_t *ps_info = SCARG(uap, pidlist);
	int no_mutex_to_send = SCARG(uap, nlist);

	// should check allocated memory is of actual size (nlist)

	int i = 0;
	struct mymutex *varmutex;
	LIST_FOREACH(varmutex, &mlist, link)
	{
		if(varmutex -> is_locked == 0)
		{
			struct myprocess *iterproc;
			LIST_FOREACH(iterproc, &(varmutex->plist), link)
			{
				if(iterproc -> waiting == 1)
				{
					if(i >= no_mutex_to_send)
					{
						*retval = -1;
						return 0;
					}

					mtx_info[i] = varmutex -> d;
					ps_info[i] = iterproc -> id;
					i++;
				}
			}
		}
	}

	*retval = i;
	return 0;
}

int	sys_mtxgrant(struct proc *p, void *v, register_t *retval)
{
	if(mlist_is_init == 0) {
		LIST_INIT(&mlist);
		mlist_is_init = 1;
	}
	struct sys_mtxgrant_args *arg = v;

	pid_t my_ps = SCARG(arg, selected);
	int associated_mutex = SCARG(arg, d);

	struct mymutex *it;
    LIST_FOREACH(it, &mlist, link) {
		if (it->d == associated_mutex) {
			it->is_locked = 1;
			break;
		}
	}

	printf("GRANT: mutex id %d to proc %d\n", associated_mutex, my_ps);

	wake_pid = my_ps;
	wakeup(&t_pause);

	*retval = 0;
	return 0;
}
