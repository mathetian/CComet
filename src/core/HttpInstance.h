// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_INSTANCE_H
#define _HTTP_INSTANCE_H

#include "Socket.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "MsgHandler.h"
using namespace sealedserver;

#include "Subscriber.h"

class HttpInstance :public MSGHandler
{
    typedef map<string, string> Params;

public:
    HttpInstance(EventLoop *loop, Socket sock);
    virtual ~HttpInstance();

public:
    /// We override receivedMsg functions in this class
    /// As we need to process the message we received
    void receivedMsg(STATUS status, Buffer &receivedBuff);

    /// Invoke when the socket has been closed
    void closed(ClsMtd st); 

public:
    /// Attach the subscriber to `this`
    void attachSubscriber(const Subscriber *const subscriber);

private:
    /// Param paramters
    void parse(Buffer &buf);
    void parseSub(const string &msg);
    void parseParams(const string &msg);
    void parseKey(const string &msg);

private:
    /// Helper functions
    bool inSet(string str);

private:
    string      type_;
    int         errcode_;
    Params      params_;
    int         clsStatus_;
    Subscriber *subscriber_;
};

#endif