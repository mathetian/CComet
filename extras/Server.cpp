#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <vector>
using namespace std;

#define BASE_PORT 7000
#define DEFAULT_NUM 10

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

vector<int> createServer(int num = DEFAULT_NUM)
{
    struct sockaddr_in addr;
    const char *ip = "0.0.0.0";
    int opt = 1;
    vector<int> serv_socks(num,-1);

    for(int i=0; i<num; i++)
    {
        int port = BASE_PORT + i;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons((short)port);
        inet_pton(AF_INET, ip, &addr.sin_addr);

        int serv_sock;
        if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            goto sock_err;
        set_blocking(serv_sock, false);
        if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
            goto sock_err;
        if(bind(serv_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
            goto sock_err;
        if(listen(serv_sock, 1024) == -1)
            goto sock_err;

        printf("server listen on port: %d\n", port);
        serv_socks[i] = serv_sock;
    }

    return serv_socks;

sock_err:
    printf("error: %s\n", strerror(errno));
    return serv_socks;
}

bool inset(int fd, vector<int> &serv_socks)
{
    int num = serv_socks.size();
    for(int i=0; i<num; i++)
    {
        if(fd == serv_socks[i])
            return true;
    }
    return false;
}

int main(int argc, char **argv)
{
    int bufsize, connections = 0;
    socklen_t optlen;
    int maxfd;
    struct sockaddr_in addr;

    {
        const char *ip = "0.0.0.0";

        int port = BASE_PORT;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons((short)port);
        inet_pton(AF_INET, ip, &addr.sin_addr);
    }

    vector<int> serv_socks = createServer(DEFAULT_NUM);
    optlen = sizeof(bufsize);
    fd_set readset;
    FD_ZERO(&readset);

    for(int i=0; i<DEFAULT_NUM; i++)
    {
        if(serv_socks[i] == -1)
            goto sock_err;
        getsockopt(serv_socks[i], SOL_SOCKET, SO_RCVBUF, &bufsize, &optlen);
        printf("default send/recv buf size: %d\n", bufsize);
        FD_SET(serv_socks[i], &readset);
        maxfd = serv_socks[i] > maxfd ? serv_socks[i] : maxfd;
    }

    setlimit(100000);

    while(1)
    {
        fd_set rr = readset;

        int ret = select(maxfd + 1, &rr, NULL, NULL, NULL);
        if(ret < 0)
        {
            if(errno == EINTR) continue;
            else
            {
                printf("select error! %s\n", strerror(errno));
                exit(0);
            }
        }

        if(ret == 0) continue;

        for(int i=0; i<=maxfd; i++)
        {
            if(!FD_ISSET(i,&rr) || inset(i, serv_socks))
                continue;

            char buf[1024];
            memset(buf, 0, 1024);

            while(true)
            {
                int count = ::read(i, buf, 1024);
                printf("read %d\n",count);
                if(count < 0 && errno == EAGAIN)
                {
                    printf("EWouldBlock\n");
                    break;
                }
                else if (count < 0) goto sock_err;
                else if (count == 0)
                {
                    ::close(i);
                    printf("%d closed\n", i);
                    FD_CLR(i, &readset);
                }
            }
        }

        for(int i=0; i < DEFAULT_NUM; i++)
        {
            if(FD_ISSET(serv_socks[i], &rr) == true)
            {
                socklen_t addrlen = sizeof(addr);
retry:
                int sock = accept(serv_socks[i], (struct sockaddr *)&addr, &addrlen);
                printf("accept %d\n",sock);
                if(sock < 0)
                {
                    if (errno == EAGAIN || errno == ECONNABORTED)
                        goto retry;
                    else
                        goto sock_err;
                }

                connections++;
                printf("connections: %d, fd: %d, through fd: %d\n", connections, sock, serv_socks[i]);
                set_blocking(sock, false);
                FD_SET(sock, &readset);
                maxfd = sock > maxfd ? sock : maxfd;
            }
        }
    }

sock_err:
    printf("error: %s\n", strerror(errno));
    return 0;
}