// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Header.h"

#include "Socket.h"
#include "Acceptor.h"
#include "EventPool.h"
#include "MsgHandler.h"
using namespace sealedserver;

#define BASE_PORT 10000
#define PORT_NUM  10

#define CLIENT_NUM 200000

EventPool pool(1);

int num;

int globalid = 0;

class EchoClient : public MSGHandler
{
public:
    EchoClient(EventLoop* loop, Socket sock) : MSGHandler(loop, sock)
    {
        m_first = true;
        registerWrite();
    }

    ~EchoClient() { }

protected:
    /// Invoked when a connection we try either succeeds or fails.
    virtual void connected()
    {
       char buf[1024];
       
       sprintf(buf, "GET /subscribe?channel=%s%d&sname=%d&callback=%s&seqid=0 sss", \
            "channelname", rand()%10000, globalid, "callback");
       
       globalid++; write(buf);
    }

    /// Invoked when a message is received
    virtual void receivedMsg(STATUS status, Buffer &receivedBuff)
    {
        if(status == SUCC)
        {
            DEBUG << "Received(from " <<  m_sock.fd() <<  "):" << (string)receivedBuff;
        }
        else assert(0);
    }

    /// Invoked when a msg has been sent
    virtual void sentMsg(STATUS status, int len, int targetLen)
    {
        if(status == SUCC)
        {
            DEBUG << "SendedMsg(to " <<  m_sock.fd() <<  "):" << len << " " << targetLen;
        }
        else assert(0);
    }

    // Invoke when the socket has been closed
    virtual void closed(ClsMtd st)
    {
        DEBUG << "onCloseSocket(for " <<  m_sock.fd() <<  "):" << st;
        if(errno != 0) DEBUG << strerror(errno);

        errno = 0;
    }
};

/// Simulator of clients
/// In it, we will try to create `CLIENT_NUM`s clients (EchoClient)
/// Then, try to create `EchoClient` to bind the socket with this client

class ClientSimulator
{
public:
    ClientSimulator(string ip, int port)
    {
        createClients(ip, port);
    }

    ClientSimulator(int port)
    {
        createClients("127.0.0.1", port);
    }

private:
    void createClients(string ip, int port)
    {
        for(int i = 0; i < CLIENT_NUM; i++)
        {
            NetAddress svrAddr(ip, port + (i%PORT_NUM));

            Socket sock(AF_INET, SOCK_STREAM);
            sock.connect(&svrAddr);

            EchoClient *client = new EchoClient(pool.getRandomLoop(), sock);

            if(i % 10000==0)
            {
                printf("press Enter to continue: ");
                getchar();
            }

            usleep(1 * 1000);
        }
    }
};

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manually";
    pool.stop();
}

/// Change the configure
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
     Log::setLevel(Log::info);

    ::signal(SIGINT, signalStop);
    setlimit(100000);
    errno = 0;

    pool.subrun();

    ClientSimulator simulator(BASE_PORT);
    
    pool.subjoin();

    INFO << "End of Main";
    
    return 0;
}