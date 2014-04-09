#include <iostream>
using namespace std;

#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "core/EventLoop.h"
#include "core/MsgHandler.h"
#include "core/Socket.h"
#include "core/Acceptor.h"

#define PORT 9000

EventLoop loop;

class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop& loop, Socket sock) : MSGHandler(loop, sock,1)
    { 
    }

    ~EchoServer()
    { }

private:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC)
        {
            INFO << "Received: " << (string)buf << " through fd " << m_sock.get_fd();
            write(buf);
        }

    }

    virtual void sendedMsg(STATUS status, int len, int targetLen)
    {
        if(status == SUCC)
        {
            INFO << "SendedMsg: " << len << " " << targetLen << " through fd " << m_sock.get_fd();
        }
        else { }
    }

    virtual void closedSocket()
    { }
};

void signalStop(int)
{
    INFO << "Stop running...by manually";
    loop.stop();
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

int main()
{
    ::signal(SIGINT, signalStop);
    setlimit(100000);
    errno = 0;
    TCPAcceptor<EchoServer> acceptor0(loop, PORT+0);
    TCPAcceptor<EchoServer> acceptor1(loop, PORT+1);
    TCPAcceptor<EchoServer> acceptor2(loop, PORT+2);
    TCPAcceptor<EchoServer> acceptor3(loop, PORT+3);
    TCPAcceptor<EchoServer> acceptor4(loop, PORT+4);
    TCPAcceptor<EchoServer> acceptor5(loop, PORT+5);
    TCPAcceptor<EchoServer> acceptor6(loop, PORT+6);
    TCPAcceptor<EchoServer> acceptor7(loop, PORT+7);
    TCPAcceptor<EchoServer> acceptor8(loop, PORT+8);
    TCPAcceptor<EchoServer> acceptor9(loop, PORT+9);

    loop.runforever();

    return 0;
}