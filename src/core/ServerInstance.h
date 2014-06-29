// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SER_INST_H
#define _SER_INST_H

#include "HttpServer.h"
using namespace sealedserver;

#include "ReqInstance.h"

namespace ccomet
{

class ServerInstance : public HttpServer
{
public:
	/// Constructor
	ServerInstance(int port): HttpServer(port, 0)
	{
		acceptor_ = new HttpAcceptor<ReqInstance>(this, pool_.getRandomLoop(), port); 
	}

	/// Destructor
    virtual ~ServerInstance()
    {
    	if(acceptor_ != NULL)
		{
			delete acceptor_;
		}

		acceptor_ = NULL;
    }

public:
	HttpAcceptor<ReqInstance> *acceptor_;
};

};

#endif