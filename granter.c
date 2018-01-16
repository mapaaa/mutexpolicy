#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

const int MTX_OPEN = 331;
const int MTX_CLOSE = 332;
const int MTX_LOCK = 333;
const int MTX_UNLOCK = 334;
const int MTX_LIST = 335;
const int MTX_GRANT = 336;

int main(int argc, char** argv) {
	if(argc != 3) {
		return 0;
	}
	int d = atoi(argv[1]);
	pid_t pid = atoi(argv[2]);
	
	printf("freeing mutex id %d from proc %d\n", d, pid);
	
	syscall(MTX_GRANT, d, pid);
	return 0;
}
