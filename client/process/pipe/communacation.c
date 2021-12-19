#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INPUT 0
#define OUTPUT 1

int main()
{
    int fds[2];
    pid_t pid;
    char buf[256];
    char content[] = "test data";

    pipe(fds);

    if ((pid = fork()) == -1) {
        printf("fork failed\n");
        return -1;
    }
    if (pid == 0) {  // child
        printf("child\n");
        close(fds[INPUT]);
        write(fds[OUTPUT], content, strlen(content) + 1);
        exit(0);
    } else {  // parent
        close(fds[OUTPUT]);
        read(fds[INPUT], buf, sizeof(buf));
        printf("parent\n");
        printf("%s\n", buf);
    }
    return 0;
}
