// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SERVER_H
#define _SERVER_H

#include "Noncopyable.h"
using namespace utils;

#include "Channel.h"
#include "HttpInstance.h"

enum STATUS1 {SUCCEEED = 0, ERRPARAM, ERRCHANL, ERRDULPE, NEEDCLSD};

class Server : public Noncopyable
{
public:
    typedef map<string, string> Params;

public:
    /// Singleton
    static Server& Instance();

    /// Destructor
    virtual ~Server();

private:
    /// Constructor
    Server();

public:
    /// We provide three functions in the server

    /// Sign, sign in with the server
    /// Send a token back to the `user`
    /// It will send a message to all active users
    /// 
    /// @param params , params   
    ///        handler, the instance of Handler
    STATUS1 sign(Params &keys, HttpInstance* handler);
        
    /// Publish, publish a message
    /// Send a message to all active users
    /// And add this messge to history
    ///
    /// @param params , params   
    ///        handler, the instance of Handler
    STATUS1 publish(Params &keys, HttpInstance* handler);
    
    /// Subscribe, subscribe with special channel
    /// Wait for reply
    ///
    /// @param params , params   
    ///        handler, the instance of Handler
    STATUS1 subscribe(Params &keys, HttpInstance* handler);

private:
    /// GetChannel with the channel name(`cname`)
    ///
    /// @reuturn null, if not found
    ///          actual channel, if found
    Channel* getChannel(const string &name);

    /// Create channel with the channel name(`cname`)
    ///
    /// @reuturn the created channel
    Channel* newChannel(const string &cname);

private:
    /// All channels created since the starting of server
    /// The key of channels is the cname(channel name)
    map<string, Channel*> channels_;
};
#endif