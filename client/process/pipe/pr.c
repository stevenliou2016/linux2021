#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    FILE *rFile;
    char *buf;
    size_t len = 0;
    ssize_t count = 0;

    rFile = fopen("myfifo", "r");
    while ((count = getline(&buf, &len, rFile)) != -1) {
        printf("%s\n", buf);
    }
    fclose(rFile);

    return 0;
}
