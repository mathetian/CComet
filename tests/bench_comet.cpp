#include <iostream>
using namespace std;

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "core/EventLoop.h"
#include "core/MsgHandler.h"
#include "core/Socket.h"

#include <stdlib.h>

#define PORT 10001
#define CLIENT_NUM 1000000
#define SUBCLI     1000
#define PORTNUM 1

EventLoop loop;
int num;

class EchoClient : public MSGHandler
{
public:
    EchoClient(EventLoop* loop, Socket sock) : MSGHandler(loop, sock)
    {
        DEBUG << m_sock.getsockname() << " " << sock.get_fd();
    }

    ~EchoClient()
    { }

protected:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC)
        {
            INFO << "ReceivedMsg: " << (string)buf << " through fd " << m_sock.get_fd();

            if(num++ < CLIENT_NUM)
            {   
                NetAddress svrAddr(PORT+(rand()%10));
                Socket sock(AF_INET, SOCK_STREAM);
                sock.cliConnect(&svrAddr);
                assert(sock.get_fd() >= 0);

                EchoClient *client = new EchoClient(&loop, sock);
               
                usleep(1 * 1000);

                if(num % 10000 == 0)
                {
                    printf("%d, press Enter to continue: ", num);
                    getchar();
                }
            }
        }
    }

    virtual void sendedMsg(STATUS status, int len, int targetLen)
    {
        if(status == SUCC)
        {
            INFO << "SendedMsg: " << len << " " << targetLen << " through fd " << m_sock.get_fd();
        }
    }

    virtual void closedSocket() { }

    virtual void onConnected(STATUS status)
    {
        INFO << "Connected Successful"; char buf[1024];
        sprintf(buf, "GET /sign?channel=%s%d&sname=%d&callback=%s SSS", "channelname", rand()%10000, rand(), "callback");
        write(buf);
    }
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
        num = SUBCLI;
        for(int i=0;i<SUBCLI;i++)
        {
            NetAddress svrAddr(PORT + (rand()%10));
            Socket sock(AF_INET, SOCK_STREAM);
            sock.cliConnect(&svrAddr);
            assert(sock.get_fd() >= 0);
            EchoClient *client = new EchoClient(&loop, sock);
            usleep(1 * 1000);
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
    errno = 0;
    ClientSimulator simulator(PORT);
    loop.runforever();

    return 0;
}