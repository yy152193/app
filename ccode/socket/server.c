#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>

#define SERVER_LISTEN_PORT 55555
#define LISTEN_BACKLOG 128
#define MAX_WRITE_SIZE 64
#define MAX_READ_SIZE 16
#define QUIT_STR "quit"

void handler_wait_child(int sig, siginfo_t *si, void *unused)
{
    int wstatus;
    if (waitpid(-1, &wstatus, WNOHANG) == -1)
        perror("waitpid");

    /* Be careful when using printf in async func. */
    printf("child pid:%d goes die.\n", si->si_pid);
}

void handler_client_data(int fd)
{
    char wbuf[MAX_WRITE_SIZE];
    char rbuf[MAX_READ_SIZE];

    int rnum, wnum;

//    int fdflags = fcntl(fd, F_GETFL, 0);
//    if (fcntl(fd, F_SETFL, fdflags|O_NONBLOCK) == -1) {
//        perror("fcntl");
//        return;
//    }

    do {
        bzero(wbuf, MAX_WRITE_SIZE);
        bzero(rbuf, MAX_READ_SIZE);
        rnum = read(fd, rbuf, MAX_READ_SIZE);
        printf("Received client %d data %s\n", rnum, rbuf);

        if (strncasecmp(rbuf, QUIT_STR, strlen(QUIT_STR)) == 0) {
            return;
        }
        if (rnum == 0) strncpy(wbuf, "server:no data.", MAX_WRITE_SIZE);
        else if (rbuf[0] >= '0' && rbuf[0] <= '9') {
            strncpy(wbuf, "server:digit data.", MAX_WRITE_SIZE-1);
        } else if (rbuf[0] >= 'a' && rbuf[0] <= 'z') {
            strncpy(wbuf, "server:lower data.", MAX_WRITE_SIZE-1);
        } else if (rbuf[0] >= 'A' && rbuf[0] <= 'Z') {
            strncpy(wbuf, "server:upper char data.", MAX_WRITE_SIZE-1);
        } else {
            strncpy(wbuf, "server:other data.", MAX_WRITE_SIZE-1);
        }
        wnum = write(fd, wbuf, MAX_WRITE_SIZE-1);

    } while (1);
    //} while (strncasecmp(rbuf, QUIT_STR, strlen(QUIT_STR)) != 0);
}


int main(int argc, char *argv[])
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler_wait_child;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(-1);
    }

    int listen_fd;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("create socket");
        exit(listen_fd);
    }

    int is_reuse = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &is_reuse, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(-1);
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(SERVER_LISTEN_PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(listen_fd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) != 0) {
        perror("bind");
        exit(-1);
    }

    if (listen(listen_fd, LISTEN_BACKLOG) != 0) {
        perror("listen");
        exit(-2);
    }

    int conn_fd;
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_size = sizeof(struct sockaddr_in);
    pid_t pid;

    for (;;) {
        conn_fd = accept(listen_fd, (struct sockaddr *)&peer_addr, &peer_addr_size);
        if (conn_fd < 0) {
            perror("accept");
            continue;
        }

        //pid = fork();
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(127);
        } else if (pid == 0) {
            printf("child process\n");
            close(listen_fd);
            handler_client_data(conn_fd);
            //read(fd);
            printf("child pid:%d, getpid():%d, conn_fd:%d\n", pid, getpid(), conn_fd);
            close(conn_fd);
            exit(1);
        } else {
            printf("parent process\n");
            close(conn_fd);
            printf("parent pid:%d, getpid():%d, listen_fd:%d\n", pid, getpid(), listen_fd);
        }
    }

    close(listen_fd);
    return 0;
}
