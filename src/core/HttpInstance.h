// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpRequest.h"
using namespace sealedserver;

#include "Subscriber.h"

namespace ccomet
{

class HttpInstance : public HttpRequest
{
public:
	/// Constructor, for the server
    HttpInstance(HttpServer *server, EventLoop *loop, Socket sock) : HttpRequest(server, loop, sock)
    {
    	subscriber_ = NULL;
    }

    /// Destructor
    virtual ~HttpInstance()
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
	/// 
	Subscriber *subscriber_;
};

};