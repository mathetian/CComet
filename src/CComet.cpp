// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Log.h"
using namespace utils;

#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace sealedserver;

#include "Server.h"
using namespace ccomet;

#define      Port 8081
HttpServer   httpServer(Port);
Server      &server = Server::Instance();

void sign_handler(HttpRequest *req, HttpResponse *rep, void *arg)
{
    server.sign(req, rep);
}

void pub_handler(HttpRequest *req, HttpResponse *rep, void *arg)
{
    server.publish(req, rep);
}

void sub_handler(HttpRequest *req, HttpResponse *rep, void *arg)
{
    server.subscribe(req, rep);
}

void error_handler(HttpRequest *req, HttpResponse *rep, void *arg)
{
    server.error(req, rep);
}

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manually";
    httpServer.stop();
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
    Log::setLevel(Log::debug);

    ::signal(SIGINT, signalStop);
    setlimit(100000);

    httpServer.add("/sign", sign_handler, NULL);
    httpServer.add("/publish",  pub_handler, NULL);
    httpServer.add("/subscribe",  sub_handler, NULL);
    httpServer.error(error_handler, NULL);

    INFO << "server starting" ;
    httpServer.start();
    INFO << "server stopping" ;

    return 0;
}