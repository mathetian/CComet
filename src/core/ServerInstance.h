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
	ServerInstance(int port, int portnum): HttpServer(port, portnum, 0), portnum_(portnum)
	{
		acceptors_ = vector<HttpAcceptor<ReqInstance>*>(portnum_, NULL);

		for(int i = 0; i < portnum_ ; i++){
			acceptors_[i] = new HttpAcceptor<ReqInstance>(this, pool_.getRandomLoop(), port); 
		}
	}

	/// Destructor
    virtual ~ServerInstance()
    {
    	for(int i = 0; i < portnum_ ; i++){
    		delete acceptors_[i];
    		acceptors_[i] = NULL;
    	}
    }

public:
	vector<HttpAcceptor<ReqInstance>*> acceptors_;
	int                                portnum_;
};

};

#endif