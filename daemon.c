// Calling daemon() will detach your process from a controlling tty, close stdin/stdout,
// and will also fork & exit the parent so that your process becomes a child of init.

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    bool run_in_foreground;
    int loop_count;
    int c;
    
    opterr = 0;
    
    while ((c = getopt(argc, argv, "f")) != -1) {
        switch (c) {
            case 'f':
            run_in_foreground = true;
            break;
            
            case '?':
            printf("Unrecognized option '%c'.\n", optopt);
            return 1;
            
            default:
            abort();
        }
    }
    
    printf("This is my application...\n");
    
    if (!run_in_foreground) {
        printf("This is the last message you'll see, since I'm going to call daemon()\n");
        daemon(0, 0);
    } else {
        printf("I'm in the foreground, so you'll still see my printfs\n");
    }
    
    for (loop_count = 0; loop_count < 10; loop_count++) {
        printf("Loop count is %d\n", loop_count);
        sleep(1);
    }
    
    return 0;
}