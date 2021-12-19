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
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == SocketFD) {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if (-1 == bind(SocketFD, (const struct sockaddr *) &stSockAddr,
                   sizeof(struct sockaddr_in))) {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == listen(SocketFD, 10)) {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    for (;;) {
        int ConnectFD = accept(SocketFD, NULL, NULL);

        if (0 > ConnectFD) {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        char buf[80];

        read(ConnectFD, buf, 80);
        printf("%s\n", buf);
        if (strncmp(buf, "quit", 4) == 0) {
            write(ConnectFD, "ok", 2);
            shutdown(ConnectFD, SHUT_RDWR);
            close(ConnectFD);
            break;
        }


        /* perform read write operations ... */
        shutdown(ConnectFD, SHUT_RDWR);
        close(ConnectFD);
    }

    close(SocketFD);
    return 0;
}
