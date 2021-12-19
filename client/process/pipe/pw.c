#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
    FILE *wFile;
    char buf[80];

    wFile = fopen("myfifo", "w");
    if (NULL == wFile) {
        printf("error opening file\n");
        return 0;
    }
    sprintf(buf, "test file\n");
    fwrite(buf, 1, strlen(buf), wFile);
    fclose(wFile);

    return 0;
}
