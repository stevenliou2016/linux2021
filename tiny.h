#ifndef _TINY_H
#define _TINY_H
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define LISTENQ 1024 /* second argument to listen() */
#define MAXLINE 1024 /* max length of a line */
#define WEB_RIO_BUFSIZE 1024

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 9999 /* use this port if none given as arg to main() */
#endif

#ifndef FORK_COUNT
#define FORK_COUNT 4
#endif

#ifndef NO_LOG_ACCESS
#define LOG_ACCESS
#endif

typedef struct {
    int rio_fd;                    /* descriptor for this buf */
    int rio_cnt;                   /* unread byte in this buf */
    char *rio_bufptr;              /* next unread byte in this buf */
    char rio_buf[WEB_RIO_BUFSIZE]; /* internal buffer */
} web_rio_t;

/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

typedef struct {
    char filename[512];
    off_t offset; /* for support Range */
    size_t end;
} http_request;

typedef struct {
    const char *extension;
    const char *mime_type;
} mime_map;

int open_listenfd(int);
void process(int, struct sockaddr_in *);
void print_help();

#endif
