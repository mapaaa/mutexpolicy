#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define max_proces 1000

const int MTX_OPEN = 331;
const int MTX_CLOSE = 332;
const int MTX_LOCK = 333;
const int MTX_UNLOCK = 334;
const int MTX_LIST = 335;
const int MTX_GRANT = 336;

int last_modify[max_proces];

int main(int argc, char** argv)
{
	FILE *fp = NULL;
	pid_t process_id = 0;
	pid_t sid = 0;

	// Create child process
	process_id = fork();
	
	// Indication of fork() failure
	if (process_id < 0)
	{
		printf("fork failed!\n");
		exit(1);
	}

	if (process_id > 0)
	{
		// PARENT PROCESS. Need to kill it.
		printf("process_id of child process %d \n", process_id);
		// return success in exit status
		exit(0);
	}

	// unmask the file mode
	umask(0);
	
	// set new session
	sid = setsid();
	
	if(sid < 0)
	{
		exit(1);
	}
	
	// Change the current working directory to root.
	chdir("/");
	
	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	// fp = fopen ("Log.txt", "w+");

	pid_t *p = malloc(max_proces * sizeof(pid_t));
	int *d = malloc(max_proces * sizeof(int));

	memset(last_modify, 0, sizeof(last_modify));

	int check_step = 0;

	while (1)
	{
		// Dont block context switches, let the process sleep for some time
		sleep(2);

		// fprintf(fp, "Logging info...\n");
		// fflush(fp);

		int many = syscall(MTX_LIST, d, p, max_proces);

		int i;
		for(i = 1; i < many; i++) {
			int swap_index = rand()%i;

			int aux = d[swap_index];
			d[swap_index] = d[i];
			d[i] = aux;

			aux = p[swap_index];
			p[swap_index] = p[i];
			p[i] = aux;
		}

		// versioning last_modify
		check_step ++;

		for(i = 0; i < many; i++) {
			if(last_modify[d[i]] != check_step) {
				last_modify[d[i]] = check_step;
				syscall(MTX_GRANT, d[i], p[i]);
			}
		}
	}

	// fclose(fp);
	return 0;
}