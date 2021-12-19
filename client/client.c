#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "rio.h"

char *server_ip;
char *port;
unsigned int buf_max_size = 1024;
bool is_mem_suc(char *);

static void print_help()
{
    printf("\nWEB CLIENT HELP\n\n");
    printf("\tclient -h		#usage\n\n");
    printf("\tclient -c serverIP	#connect to server\n\n");
    printf("\tclient -f fileName	#download file\n\n");
    printf("\tclient -p port\t	#port\n\n");
    printf("\tclient -d directory	#download directory\n\n");
}

static bool download_file(char *file_name)
{
    web_rio_t rio;
    int n = 1;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof server_addr;
    int connect_fd = -1;
    FILE *p_file;
    bool is_writing = false;
    char *server_buf = calloc(buf_max_size, sizeof(char));
    char *buf = calloc(2 * buf_max_size, sizeof(char));

    if (!is_mem_suc(server_buf)) {
        return -1;
    }
    if (!is_mem_suc(buf)) {
        return -1;
    }

    p_file = fopen(file_name, "w");

    if ((connect_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    uint32_t u_port = strtoul(port, NULL, 10);
    server_addr.sin_port = htons(u_port);
    int res = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if (res < 0) {
        printf("error: first parameter is not a valid address family\n");
        close(connect_fd);
        return false;
    } else if (res == 0) {
        printf(
            "char string (second parameter does not contain valid ipaddress\n");
        close(connect_fd);
        return false;
    }

    if (connect(connect_fd, (const struct sockaddr *) &server_addr,
                sizeof(struct sockaddr_in)) == -1) {
        printf("connect failed\n");
        printf("%d\n", __LINE__);
        printf("%d\n", errno);
        close(connect_fd);
        return false;
    }

    sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s:%s\r\n%s%s%s%s%s%s%s", file_name,
            server_ip, port, "Connection: keep-alive\r\n",
            "Cache-Control: max-age=0\r\n", "Upgrade-Insecure-Requests: 1\r\n",
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
            "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.93 "
            "Safari/537.36\r\n",
            "Accept: "
            "text/html,application/xhtml+xml,application/xml;q=0.9,image/"
            "avif,image/webp,image/apng,*/*;q=0.8,application/"
            "signed-exchange;v=b3;q=0.9\r\n",
            "Accept-Encoding: gzip, deflate\r\n",
            "Accept-Language: zh-TW,zh;q=0.9\r\n\r\n");
    writen(connect_fd, buf, strlen(buf));

    rio_read_init(&rio, connect_fd);
    while (n > 0) {
        n = rio_read_line(&rio, server_buf, MAXLINE);

        if (strncmp(server_buf, "Content-type", 12) == 0) {
            rio_read_line(&rio, server_buf, MAXLINE);
            is_writing = true;
            continue;
        }

        if (p_file == NULL) {
            printf("open failure\n");
            return false;
        }

        if (is_writing) {
            fwrite(server_buf, 1, n, p_file);
        }
    }
    fclose(p_file);
    close(connect_fd);

    return true;
}

bool download_dir(char *dir_name)
{
    if (access(dir_name, F_OK) != 0) {
        if (mkdir(dir_name, S_IRWXU) == -1) {
            printf("make a directory failed\n");
            return false;
        }
    }

    web_rio_t rio;
    int n = 1;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof server_addr;
    int connect_fd = -1;
    char *buf = calloc(buf_max_size, sizeof(char));
    char *server_buf = calloc(buf_max_size, sizeof(char));

    if (!is_mem_suc(buf)) {
        return false;
    }

    if (!is_mem_suc(server_buf)) {
        return false;
    }

    if ((connect_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return false;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    uint32_t u_port = strtoul(port, NULL, 10);
    server_addr.sin_port = htons(u_port);
    int res = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if (res < 0) {
        printf("error: first parameter is not a valid address family\n");
        close(connect_fd);
        return false;
    } else if (res == 0) {
        printf(
            "char string (second parameter does not contain valid ipaddress\n");
        close(connect_fd);
        return false;
    }

    if (connect(connect_fd, (const struct sockaddr *) &server_addr,
                sizeof(struct sockaddr_in)) == -1) {
        printf("connect failed\n");
        close(connect_fd);
        return false;
    }

    sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s:%s\r\n%s%s%s%s%s%s%s", dir_name,
            server_ip, port, "Connection: keep-alive\r\n",
            "Cache-Control: max-age=0\r\n", "Upgrade-Insecure-Requests: 1\r\n",
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
            "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.93 "
            "Safari/537.36\r\n",
            "Accept: "
            "text/html,application/xhtml+xml,application/xml;q=0.9,image/"
            "avif,image/webp,image/apng,*/*;q=0.8,application/"
            "signed-exchange;v=b3;q=0.9\r\n",
            "Accept-Encoding: gzip, deflate\r\n",
            "Accept-Language: zh-TW,zh;q=0.9\r\n\r\n");
    writen(connect_fd, buf, strlen(buf));

    int file_index = 0;
    int dir_index = 0;
    char **file_array = calloc(buf_max_size, sizeof(char *));
    char **dir_array = calloc(buf_max_size, sizeof(char *));
    char *name = calloc(buf_max_size, sizeof(char));
    char *target = calloc(buf_max_size, sizeof(char));

    if (file_array == NULL) {
        printf("malloc failed\n");
        return false;
    }

    if (dir_array == NULL) {
        printf("malloc failed\n");
        return false;
    }

    if (!is_mem_suc(name)) {
        return false;
    }

    if (!is_mem_suc(target)) {
        return false;
    }

    rio_read_init(&rio, connect_fd);

    while (0 < n) {
        memset(server_buf, 0, sizeof(server_buf));
        n = rio_read_line(&rio, server_buf, MAXLINE);

        if (strncmp(server_buf, "<tr><td><a href=", 16) == 0) {
            char *p = server_buf;
            int i = 0;
            p += 17;

            memset(name, 0, sizeof(name));
            memset(target, 0, sizeof(target));

            while (*p != '"') {
                name[i] = *p;
                i++;
                p++;
            }
            name[i] = '\0';
            if (!is_mem_suc(target)) {
                return -1;
            }
            strcat(target, dir_name);
            p = target;
            while (*p != '\0') {
                p++;
            }
            p--;
            if (*p != '/') {
                strcat(target, "/");
            }
            strcat(target, name);
            if (name[i - 1] == '/') {
                if (dir_index < buf_max_size) {
                    dir_array[dir_index] = calloc(buf_max_size, sizeof(char));
                    if (!is_mem_suc(dir_array[dir_index])) {
                        return false;
                    }
                    memcpy(dir_array[dir_index++], target, buf_max_size);
                } else {
                    buf_max_size *= 2;
                    char **dp = realloc(dir_array, buf_max_size);
                    if (dp == NULL) {
                        printf("malloc failed\n");
                        return false;
                    }
                    dir_array = dp;
                }
            } else {
                if (file_index < buf_max_size) {
                    file_array[file_index] = calloc(buf_max_size, sizeof(char));
                    if (!is_mem_suc(file_array[file_index])) {
                        return false;
                    }
                    memcpy(file_array[file_index++], target, buf_max_size);
                } else {
                    buf_max_size *= 2;
                    char **fp = realloc(file_array, buf_max_size);
                    if (fp == NULL) {
                        printf("malloc failed\n");
                        return false;
                    }
                    file_array = fp;
                }
            }
        }
    }
    close(connect_fd);
    int i = 0;
    while (file_array[i] != NULL) {
        download_file(file_array[i++]);
    }
    i = 0;
    while (dir_array[i] != NULL) {
        download_dir(dir_array[i++]);
    }

    return true;
}

bool is_mem_suc(char *s)
{
    if (s == NULL) {
        printf("malloc failed\n");
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    int ch = 'h';
    int max_size = 64;
    bool is_file = false;
    bool is_dir = false;
    char *file_name;
    char *dir_name;

    // init
    file_name = (char *) malloc(max_size);
    if (!is_mem_suc(file_name)) {
        return -1;
    }
    memset(file_name, 0, sizeof(file_name));
    dir_name = (char *) malloc(max_size);
    if (!is_mem_suc(dir_name)) {
        return -1;
    }
    strcpy(file_name, "qtest.c");
    memset(dir_name, 0, sizeof(dir_name));
    server_ip = calloc(16, sizeof(char));
    if (!is_mem_suc(server_ip)) {
        return -1;
    }
    strncpy(server_ip, "140.118.155.192", 16);
    server_ip[15] = '\0';
    port = calloc(5, sizeof(char));
    if (!is_mem_suc(port)) {
        return -1;
    }
    strncpy(port, "9999", 5);

    while ((ch = getopt(argc, argv, "hf:c:p:d:")) != -1) {
        switch (ch) {
        case 'h':
            print_help();
            break;
        case 'f':
            if (strlen(argv[optind - 1]) > max_size) {
                max_size *= 2;
                char *f = realloc(file_name, max_size);
                if (!is_mem_suc(f)) {
                    return -1;
                }
                file_name = f;
            }
            memcpy(file_name, argv[optind - 1], strlen(argv[optind - 1]));
            is_file = true;
            break;
        case 'c':
            memcpy(server_ip, argv[optind - 1], strlen(argv[optind - 1]));
            break;
        case 'p':
            memcpy(port, argv[optind - 1], strlen(argv[optind - 1]));
            break;
        case 'd':
            if (strlen(argv[optind - 1]) > max_size) {
                char *dp = realloc(dir_name, max_size);
                if (!is_mem_suc(dp)) {
                    return -1;
                }
            }
            memcpy(dir_name, argv[optind - 1], strlen(argv[optind - 1]));
            is_dir = true;
            break;
        default:
            printf("Unknown option\n");
            break;
        }
    }

    if (is_file || (!is_file && !is_dir)) {
        if (!download_file(file_name)) {
            printf("download %s failed\n", file_name);
            return -1;
        } else
            printf("download %s sucessfully\n", file_name);
    }
    if (is_dir) {
        if (!download_dir(dir_name)) {
            printf("download directory %s failed\n", dir_name);
            return -1;
        } else
            printf("download directory %s sucessfully\n", dir_name);
    }
    return 0;
}
