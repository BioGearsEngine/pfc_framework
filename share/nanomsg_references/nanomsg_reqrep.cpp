#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#define SERVER "reqrep_server"
#define CLIENT "reqrep_client"
#define DATE "DATE"

void
fatal(const char *func)
{
        fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
        exit(1);
}

char *
date(void)
{
        time_t now = time(&now);
        struct tm *info = localtime(&now);
        char *text = asctime(info);
        text[strlen(text)-1] = '\0'; // remove '\n'
        return (text);
}

int
server(const char *url)
{
        int sz_date = strlen(DATE) + 1; // '\0' too
        int sock;
        int rv;

        if ((sock = nn_socket(AF_SP, NN_REP)) < 0) {
                fatal("nn_socket");
        }
          if ((rv = nn_bind(sock, url)) < 0) {
                fatal("nn_bind");
        }
        for (;;) {
                char *buf = NULL;
                int bytes;
                if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0) {
                        fatal("nn_recv");
                }
                if ((bytes == (strlen(DATE) + 1)) && (strcmp(DATE, buf) == 0)) {
                        printf("server: RECEIVED DATE REQUEST\n");
                        char *d = date();
                        int sz_d = strlen(d) + 1; // '\0' too
                        printf("server: SENDING DATE %s\n", d);
                        if ((bytes = nn_send(sock, d, sz_d, 0)) < 0) {
                                fatal("nn_send");
                        }
                }
                nn_freemsg(buf);
        }
}

int
client(const char *url)
{
        int sz_date = strlen(DATE) + 1; // '\0' too
        char *buf = NULL;
        int bytes = -1;
        int sock;
        int rv;

        if ((sock = nn_socket(AF_SP, NN_REQ)) < 0) {
                fatal("nn_socket");
        }
        if ((rv = nn_connect (sock, url)) < 0) {
                fatal("nn_connect");
        }
        printf("client: SENDING DATE REQUEST %s\n", DATE);
        if ((bytes = nn_send(sock, DATE, sz_date, 0)) < 0) {
                fatal("nn_send");
        }
        if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0) {
                fatal("nn_recv");
        }
        printf("client: RECEIVED DATE %s\n", buf);  
        nn_freemsg(buf);
        return (nn_shutdown(sock, 0));
}

int
main(const int argc, const char **argv)
{
        if ((argc > 1) && (strcmp(server, argv[1]) == 0))
                return (server(argv[2]));

        if ((argc > 1) && (strcmp(client, argv[1]) == 0))
                return (client(argv[2]));

      fprintf(stderr, "Usage: reqrep %s|%s <URL> ...\n", SERVER, CLIENT);
      return (1);
}