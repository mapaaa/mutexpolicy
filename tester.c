#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define t 10
#define s 1

const int MTX_OPEN = 331;
const int MTX_CLOSE = 332;
const int MTX_LOCK = 333;
const int MTX_UNLOCK = 334;
const int MTX_LIST = 335;
const int MTX_GRANT = 336;

void play()
{
	char x0 = 'a' + rand()%26;
	char x1 = 'a' + rand()%26;

	while(x0 == x1)
	{
		x1 = 'a' + rand()%26;
	}

	char s1[] = {x0, 0};
	char s2[] = {x1, 0};

	printf("requesting %c and %c\n", x0, x1);

	int d[2];

	d[0] = syscall(MTX_OPEN, s1);
	d[1] = syscall(MTX_OPEN, s2);

	int i;
	for(i = 0; i < s; i++)
	{
		int chosen_d = d[rand()&1];

		syscall(MTX_LOCK, chosen_d);

		sleep(1);

		syscall(MTX_UNLOCK, chosen_d);
	}

	syscall(MTX_CLOSE, d[0]);
	syscall(MTX_CLOSE, d[1]);
}

int main()
{
	pid_t dd[t];

	int i;

	for(i = 0; i < t; i++)
	{
		dd[i] = fork();

		if(dd[i] == 0)
		{
			play();
			return 0;
		}
	}

	pid_t *p = malloc(1000 * sizeof(pid_t));
	int *d = malloc(1000 * sizeof(int));

	for(i = 0; i < t; i++)
	{
		int many = syscall(MTX_LIST, d, p, 1000);

		if(many < 0) {
			printf("error\n");
			return 0;
		}

		int i;
		for(i = 0; i < many; i++) {
			printf("awaiting pair mutex %d proc %d\n", d[i], p[i]);
		}
		printf("\n\n");
		fflush(stdout);
		
		pid_t child = wait(NULL);
		printf("child %d returned\n", child);
	}

	return 0;
}
