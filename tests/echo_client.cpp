#include <iostream>
using namespace std;

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../core/Socket.h"
#include "../core/EventLoop.h"
#include "../core/MsgHandler.h"

#define PORT 10000
#define CLIENT_NUM 1000

EventLoop loop;

class EchoClient : public MSGHandler
{
public:
    EchoClient(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
        write("wait for me");
    }

    ~EchoClient()
    { }

protected:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC) 
        { 
            INFO << "ReceivedMsg: " << (string)buf << " through fd " << m_sock.get_fd();
        }
    }

    virtual void sendedMsg(STATUS status, int len, int targetLen)
    {
        if(status == SUCC) 
        { 
            INFO << "SendedMsg: " << len << " " << targetLen << " through fd " << m_sock.get_fd();
        }
    }

    virtual void closedSocket()
    { }
};

class ClientSimulator
{
public:
    ClientSimulator(string ip, int port)
    {
        Initialize(ip, port);
    }

    ClientSimulator(int port)
    {
        Initialize(port);
    }

    void Initialize(int port)
    {
        NetAddress svrAddr(port);
        createClients(&svrAddr, CLIENT_NUM);
    }

    void Initialize(string ip, int port)
    {
        NetAddress svrAddr(ip, port);
        createClients(&svrAddr, CLIENT_NUM);
    }

private:
    void createClients(Address *psvrAddr, int size)
    {
        for(int i = 0; i < size; i++)
        {
            Socket sock(AF_INET, SOCK_STREAM);
            sock.cliConnect(psvrAddr);
            assert(sock.get_fd() >= 0);
            EchoClient *client = new EchoClient(loop, sock);
        }
    }
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
    
    ClientSimulator simulator(PORT);
    loop.runforever();

    return 0;
}