// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SERVER_H
#define _SERVER_H

#include "Noncopyable.h"
using namespace utils;

#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace http;

#include "Channel.h"

namespace ccomet
{

class Server : public Noncopyable
{
public:
    typedef enum {SUCCEEED = 0, ERRPARAM, ERRCHANL, ERRDULPE, NEEDCLSD} ReqStatus;

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
    /// /sign?channel=channel1&sname=user1&callback=callback
    ///
    /// @param req , HttpRequest
    ///        rep , HttpResponse
    ReqStatus sign(HttpRequest *req, HttpResponse *rep);

    /// Publish, publish a message
    /// Send a message to all active users
    /// And add this messge to history
    ///
    /// /publish?channel=channel1&sname=user2&callback=callback&msg=helloworld
    ///
    /// @param req , HttpRequest
    ///        rep , HttpResponse
    ReqStatus publish(HttpRequest *req, HttpResponse *rep);

    /// Subscribe, subscribe with special channel
    /// Wait for reply
    ///
    /// /subscribe?channel=channel1&sname=user1&seqid=0&callback=callback
    ///
    /// @param req , HttpRequest
    ///        rep , HttpResponse
    ReqStatus subscribe(HttpRequest *req, HttpResponse *rep);

    /// error query string
    ReqStatus error(HttpRequest *req, HttpResponse *rep);

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
    /// Helper function for error in params
    void     errorInParams(HttpRequest *req, HttpResponse *rep);

    /// Helper function for error in duplicated chain or others
    void     errorInOthers(HttpRequest *req, HttpResponse *rep);

    /// Helper function invoked when sign in/publish successfully
    ///
    /// @param type, 0 sign in
    ///            , 1 publish
    void     succeed(HttpRequest *req, HttpResponse *rep, int type);

private:
    /// All channels created since the starting of server
    /// The key of channels is the cname(channel name)
    map<string, Channel*> channels_;

    /// Short for params in query string
    typedef map<string, string> Params;
};

};

#endif