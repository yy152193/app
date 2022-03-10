#include <sys/socket.h>
#include <sys/types.h>
#include <error.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_LISTEN_PORT 55555
#define SERVER_IP "127.0.0.1"
#define INPUT_MAX_SIZE 16
#define OUTPUT_MAX_SIZE 64
#define QUIT_STR "quit"

int main(int argc, char *argv[])
{
    int com_fd;
    struct sockaddr_in server_addr;
    com_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (com_fd == -1) {
        perror("socket");
        exit(-1);
    }

    if (inet_pton(AF_INET, SERVER_IP, (void *)&server_addr.sin_addr) != 1) {
        perror("inet_pton");
        exit(-1);
    }

    server_addr.sin_port = htons(SERVER_LISTEN_PORT);
    server_addr.sin_family = AF_INET;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if (connect(com_fd, (struct sockaddr *)&server_addr, addrlen) == -1) {
        perror("connect");
        exit(-1);
    }

    printf("Client connect DONE.\n");

    char wbuf[INPUT_MAX_SIZE];
    char rbuf[OUTPUT_MAX_SIZE];

    int wnum, rnum;
    for (;;) {
        if (fgets(wbuf, INPUT_MAX_SIZE-1, stdin) == NULL) {
            printf("fgets NULL\n");
            continue;
        }

        bzero(wbuf, INPUT_MAX_SIZE);
        wnum = write(com_fd, wbuf, INPUT_MAX_SIZE-1);
        printf("client write %ld data:%s\n", strlen(wbuf), wbuf);

        if (0 == strncasecmp(wbuf, QUIT_STR, strlen(QUIT_STR))) {
            printf("Client quit\n");
            break;
        }

        bzero(rbuf, OUTPUT_MAX_SIZE);
        //char* rp = rbuf;
        //int ntotal = OUTPUT_MAX_SIZE;
        //do {
        //    while (nleft>0) {
        //        nbyte = read(com_fd, rp, nleft);
        //        if (nbyte<0) {
        //            // error
        //            return nbyte;
        //        } else if (nbyte == 0) {
        //            // EOF
        //            break;
        //        } else {// nbyte>0
        //            nleft = nleft - nbyte;
        //            rp += nbyte;
        //        }
        //    }
        //    ntotal = ntotal - nleft;
             
            rnum = read(com_fd, rbuf, OUTPUT_MAX_SIZE-1);
            printf("Receive server %d data %s\n", rnum, rbuf);
        } while (rnum < 0 && errno == EINTR);

        printf("Data from server:%s\n", rbuf);

    }

    close(com_fd);


    return 0;
}
