// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace sealedserver;

HttpClient client;

#define BASE_PORT 8081

#define CLIENT_NUM 10000

void get(HttpRequest *req, void *arg)
{
    HttpParser *parser = req -> getParser();

    WARN << parser -> getOrigin() ;
}

void error(HttpRequest *req, void *arg)
{
    WARN << "error" ;
}

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manually";
    client.stop();
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

    client.start();
    /// yulongti.info/?p=2761
    
    for(int i = 0; i < CLIENT_NUM ;i++){
        stringstream ss;
        ss << "127.0.0.1:8081/subscribe?channel=channel" << i << "&sname=user" << i << "&seqid=0&callback=callback";
        client.request(ss.str(), get, error, NULL);
   
    }
   
    client.wait();

    return 0;
}