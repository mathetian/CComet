// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SUB_SCRIBER_H
#define _SUB_SCRIBER_H

#include "Noncopyable.h"
using namespace utils;

#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace http;

namespace ccomet
{

class Server;
class Channel;

/**
** Each user will be a subscriber
**/
class Subscriber : public Noncopyable
{
public:
    /// Constructor
    Subscriber(string sname, int seqid, string callback, Server *server, Channel *channel,
               HttpRequest *req, HttpResponse *rep);

    /// Destructor
    virtual ~Subscriber();

public:
    /// Send single message
    void   send(const string &message);

    /// Check wthether send old data(seqid2updated id)
    ///
    /// @return false, can remove it
    ///         true , sub successfully
    bool   check();

    /// Close the handler_(detach and remove from memory)
    void   close();

public:
    /// Get the name(id) of the subscriber
    string getName() const;

private:
    /// Concat message with  `callback`
    string  concat(const string &message);

private:
    /// Each user will belong to the server
    Server  *server_;

    /// Each user will belong to a channel
    Channel *channel_;

    /// Each user will have a name(or id)
    string  sname_;

    /// Each user will keep a sequce_id(for message)
    int     seqid_;

    /// Each user will keep the name of callback in memory
    string  callback_;

    /// Http Request/Response
    HttpRequest  *req_;
    HttpResponse *rep_;
};

};

#endif