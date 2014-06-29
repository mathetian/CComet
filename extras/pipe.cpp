#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAXEVENTS 64

int
main (int argc, char *argv[])
{

    struct epoll_event event;
    struct epoll_event *events;
    int pfds[2];
    char buf[30];

    pipe(pfds);

    if (!fork())
    {
        printf(" CHILD: sleeping\n");
        sleep(3);
        printf(" CHILD: writing to the pipe\n");
        write(pfds[1], "test", 5);
        printf(" CHILD: exiting\n");
        exit(0);
    }
    else
    {
        int efd, n, s;

        // create epoll instance.
        efd = epoll_create1 (0);

        // allocate events array
        events = (epoll_event*)calloc (MAXEVENTS, sizeof event);

        // register read end of pipe using edge-triggered mode with ONESHOT.
        event.data.fd = pfds[0];
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        s = epoll_ctl (efd, EPOLL_CTL_ADD, pfds[0], &event);

        // wait on read-end becoming readable.
        printf("PARENT: waiting on pipe\n");
        n = epoll_wait (efd, events, MAXEVENTS, -1);
        printf("PARENT: epoll_wait success; n=%d.\n", n);

        // don't read anything, just re-arm the FD and wait again.
        epoll_ctl (efd, EPOLL_CTL_MOD, pfds[0], &event);
        printf("PARENT: waiting on pipe\n");
        n = epoll_wait (efd, events, MAXEVENTS, -1);
        printf("PARENT: epoll_wait success; n=%d.\n", n);

    }
    return 0;

}