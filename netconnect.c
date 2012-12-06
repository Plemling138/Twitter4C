/* Last Modified: March 13, 2012. by Plemling138 */
/*
 * Copyright (C) 2006-2008 Slogical Corporation. All Rights Reserved. 
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "hmac.h"
#include "base64.h"

static int sofd;
static struct hostent     *shost;
static struct sockaddr_in sv_addr;

int main (int argc, char *argv[]) {

    char *target_host;
    char target_uri[1024 +1];
    int  target_portno;

    char http_res[1024];

    /* ヘルプの表示 */
    if (argc >=2 && strncmp(argv[1], "-h", 2) == 0) {
        fprintf(stderr, "1st arg is HOSTNAME of httpd server (required).\n");
        fprintf(stderr, "2nd arg is URI of request (default is '/')\n");
        fprintf(stderr, "3rd arg is PORT_NO of httpd server (default is 80)\n");
        return 0;
    }

    /* target_host の設定 */
    if (argc < 2) {
        fprintf(stderr, "1st arg must be HOSTNAME of httpd server.\n");
        fprintf(stderr, "if you want to show help, run '%s -h'\n", argv[0]);
        return -1;
    }
    target_host = argv[1];

    /* target_uri の設定 */
    if (argc >= 3) {
        if (strncmp(argv[2], "/", 1) != 0) {
            fprintf(stderr, "URI of request must be started with '/'\n");
            return -1;
        }
        if (strlen(argv[2]) >= 1024) {
            fprintf(stderr, "URI of request must be under 1024bytes\n");
            return -1;
        }
        strcpy(target_uri, argv[2]);
    } else {
        strcpy(target_uri, "/");
    }

    /* target_portno の設定 */
    if (argc >= 4) {
        target_portno = atoi(argv[3]);
        if (target_portno < 0 || target_portno > 65536) {
            fprintf(stderr, "PORT_NO of httpd server is invalid\n");
            return -1;
        }
    } else {
        target_portno = 80;
    }

    /* ソケットを作成 */
    sofd = socket(AF_INET, SOCK_STREAM, 0);
    if (sofd < 0) {
        fprintf(stderr, "can not open SOCKET.\n");
        return 1;
    }

    /* アドレスを定義 */
    shost = gethostbyname(target_host);
    if (shost == NULL) {
        fprintf(stderr, "err happend in gethostbyname function.\n");
        return 1;
    }

    memset(&sv_addr, '\0', sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(target_portno);
    memcpy((char *)&sv_addr.sin_addr, (char *)shost->h_addr, shost->h_length);

    /* コネクトする */
    if (connect(sofd, (const struct sockaddr*)&sv_addr, sizeof(sv_addr)) < 0) {
        fprintf(stderr, "err happend in connect function.\n");
        return 1;
    }

    /* HTTPのやりとり */
    send(sofd, "GET ",      strlen("GET "),      0);
    send(sofd, target_uri,  strlen(target_uri),  0);
    send(sofd, " HTTP/1.0", strlen(" HTTP/1.0"), 0);
    send(sofd, "\r\n",      strlen("\r\n"),      0);
    send(sofd, "Host: ",    strlen("Host: "),    0);
    send(sofd, target_host, strlen(target_host), 0);
    send(sofd, "\r\n",      strlen("\r\n"),      0);
    send(sofd, "\r\n",      strlen("\r\n"),      0);

    while (recv(sofd, http_res, sizeof(http_res), 0)) {
        printf("%s",http_res);
        memset(&http_res, '\0', sizeof(http_res));
    }

    /* クローズする */
    close(sofd);

    return 1;
}
