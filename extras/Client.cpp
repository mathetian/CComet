#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include <sys/time.h>
#include <sys/resource.h>

#define BASE_PORT 7000
#define DEFAULT_NUM 1

bool set_blocking(int fd, bool block)
{
    int flags = fcntl(fd, F_GETFL);
    assert(flags >= 0);
    flags = block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
    assert(fcntl(fd, F_SETFL, flags) >= 0);

    return true;
}

int setlimit(int num_pipes)
{
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
    if (::setrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        fprintf(stderr, "setrlimit error: %s", strerror(errno));
        return 1;
    }
}

int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    const char *ip = "127.0.0.1";
    int opt = 1;
    int bufsize;
    socklen_t optlen;
    int connections = 0;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr.sin_addr);

    setlimit(500000);

    int index = 0;
    while(1)
    {
        int port = BASE_PORT + (index%DEFAULT_NUM);
        index++;
        addr.sin_port = htons((short)port);

        int sock;
        if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            goto sock_err;

        set_blocking(sock, false);

        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            if (errno != EINPROGRESS)
                goto sock_err;
        }

        connections ++;
        printf("connections: %d, fd: %d, port %d\n", connections, sock, port);

        if(connections % 10000 == 9999)
        {
            printf("press Enter to continue: ");
            getchar();
        }
        usleep(1 * 1000);
    }

    return 0;

sock_err:
    printf("error: %s\n", strerror(errno));
    return 0;
}