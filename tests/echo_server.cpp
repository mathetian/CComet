#include <iostream>
using namespace std;

#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../core/EventLoop.h"
#include "../core/MsgHandler.h"
#include "../core/Socket.h"
#include "../core/Acceptor.h"

#define PORT 10000

EventLoop loop;

class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    { }

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
    
    TCPAcceptor<EchoServer> acceptor(loop, PORT);
    loop.runforever();

    return 0;
}