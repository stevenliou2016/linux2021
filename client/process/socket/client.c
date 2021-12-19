#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == SocketFD) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    Res = inet_pton(AF_INET, "140.118.155.192", &stSockAddr.sin_addr);

    if (0 > Res) {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    } else if (0 == Res) {
        perror(
            "char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketFD, (const struct sockaddr *) &stSockAddr,
                      sizeof(struct sockaddr_in))) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    char wbuf[80] = "quit";
    char rbuf[80];

    write(SocketFD, wbuf, strlen(wbuf));
    read(SocketFD, rbuf, 80);
    printf("%s\n", rbuf);
    /* perform read write operations ... */

    shutdown(SocketFD, SHUT_RDWR);

    close(SocketFD);
    return 0;
}
