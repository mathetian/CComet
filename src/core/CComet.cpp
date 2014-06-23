// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Log.h"
using namespace utils;

#include "Socket.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "MsgHandler.h"
using namespace sealedserver;

#include "Server.h"
#include "HttpInstance.h"

#define PORT_NUM  10
#define BASE_PORT 10000

Server    server;
EventPool poll(4);

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manually";
    pool.stop();
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
    Log::setLevel(Log::debug);

    ::signal(SIGINT, signalStop);
    
    setlimit(100000);

    vector<TCPAcceptor<HttpInstance>*> acceptors(PORT_NUM, NULL);

    for(int i = 0; i < PORT_NUM; i++)
        acceptors[i] = new TCPAcceptor<HttpInstance>(&loop, BASE_PORT+i);

    loop.run();

    for(int i = 0; i < PORT_NUM; i++)
    {
        delete acceptors[i];
        acceptors[i] = NULL;
    }
    
    return 0;
}