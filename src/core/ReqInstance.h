// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _REQ_INST_H
#define _REQ_INST_H

#include "HttpRequest.h"
using namespace sealedserver;

#include "Subscriber.h"

namespace ccomet
{

class ReqInstance : public HttpRequest
{
public:
	/// Constructor, for the server
    ReqInstance(HttpServer *server, EventLoop *loop, Socket sock) : HttpRequest(server, loop, sock)
    {
    	subscriber_ = NULL;
    }

    /// Destructor
    virtual ~ReqInstance()
    {
    }

public:
    virtual void RegisterVariable(void *arg) 
    { 
    	subscriber_ = (Subscriber*)arg;
    }

private:
	virtual void closed(ClsMtd st)
    {
    	if(subscriber_ != NULL)
    		delete subscriber_;
    	subscriber_ = NULL;
    }

private:

	/// Subscriber
	Subscriber *subscriber_;
};

};

#endif