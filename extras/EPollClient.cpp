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

#include <map>
#include <vector>
using namespace std;

#define BASE_PORT 7000
#define DEFAULT_NUM 10
#define MAX_NEVENTS 1000000

#include <sys/epoll.h>

class Msg
{
    int fd;
    int events;
    int type;
public:
    Msg(int fd, int type) : fd(fd), events(0), type(type) { }
    int getType()
    {
        return type;
    }
    int getFD()
    {
        return fd;
    }
};

map<int, Msg*> imsgs;

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

const char *str = "hello tyl";

int main()
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

    optlen = sizeof(bufsize);

    int m_epollFD;
    struct epoll_event *events;

    assert((m_epollFD = epoll_create(1)) != -1);
    events = (struct epoll_event*)malloc(MAX_NEVENTS * sizeof(struct epoll_event));
    assert(events != NULL);

    int index = 0;
    while(1)
    {
        for(int i=0;i<100&&index<10000;i++)
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

            imsgs[sock] = new Msg(sock,1);

            struct epoll_event ev;
            memset(&ev, 0, sizeof(struct epoll_event));
            ev.data.fd = sock;
            ev.events  = EPOLLIN | EPOLLET | EPOLLOUT;

            assert(epoll_ctl(m_epollFD, EPOLL_CTL_ADD, sock, &ev) == 0);
        }

        int ret = epoll_wait(m_epollFD, events, MAX_NEVENTS, 5000);
    
        if(ret < 0)
        {
            if(errno == EINTR)
                continue;
            else
                goto sock_err;
        }
        if(ret == 0) continue;

        for(int i=0; i<ret; i++)
        {
            assert(imsgs.find(events[i].data.fd) != imsgs.end());
            Msg*msg = imsgs[events[i].data.fd];
            int what = events[i].events;

            if(what & (EPOLLHUP|EPOLLERR))
            {
                //close
                printf("Need close %d\n", msg->getFD());
                assert(epoll_ctl(m_epollFD, EPOLL_CTL_DEL, msg->getFD(), NULL) == 0);
                ::close(msg->getFD());
                imsgs.erase(msg->getFD());
                delete msg; msg = NULL;
            }
            else
            {
                if(what & EPOLLIN)
                {
                    char buf[1024];
                    memset(buf, 0, 1024);

                    while(true)
                    {
                        int count = ::read(msg->getFD(), buf, 1024);
                        printf("read %d %d\n",msg->getFD(),count);
                        if(count < 0 && errno == EAGAIN)
                        {
                            printf("EWouldBlock\n");
                            break;
                        }
                        else if (count < 0) goto sock_err;
                        else if (count == 0)
                        {
                            printf("Need Closed Socket\n");
                            assert(epoll_ctl(m_epollFD, EPOLL_CTL_DEL, msg->getFD(), NULL) == 0);
                            ::close(msg->getFD());
                            imsgs.erase(msg->getFD());
                            delete msg; msg = NULL;
                            break;
                        }
                        else
                        {
                            int ret = ::write(msg->getFD(), buf, count);
                            if(ret < 0 && errno == EAGAIN)
                            {
                                struct epoll_event ev = {0, {0}};
                                ev.data.fd = msg->getFD();
                                ev.events  = EPOLLIN | EPOLLET | EPOLLOUT;
                                assert(epoll_ctl(m_epollFD, EPOLL_CTL_MOD, msg->getFD(), &ev) == 0);
                            }
                            else if(ret < 0) goto sock_err;
                            else
                            {
                            }
                                
                        }
                    }
                }
                if(what & EPOLLOUT)
                {
                    //Todo list
                    int ret = ::write(msg->getFD(), str, strlen(str));
                    if(ret < 0 && errno == EAGAIN)
                    {
                        struct epoll_event ev = {0, {0}};
                        ev.data.fd = msg->getFD();
                        ev.events  = EPOLLIN | EPOLLET;
                        assert(epoll_ctl(m_epollFD, EPOLL_CTL_MOD, msg->getFD(), &ev) == 0);
                    }
                    else if(ret < 0) goto sock_err;
                    else
                    { 
                        struct epoll_event ev = {0, {0}};
                        ev.data.fd = msg->getFD();
                        ev.events  = EPOLLIN | EPOLLET;
                        assert(epoll_ctl(m_epollFD, EPOLL_CTL_MOD, msg->getFD(), &ev) == 0);
                    }
                }
            }

            usleep(1 * 1000);
        }
    }

sock_err:
    printf("error: %s\n", strerror(errno));
    return 0;
}