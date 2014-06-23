// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SUB_SCRIBER_H
#define _SUB_SCRIBER_H

#include <string>
using namespace std;

class Server;
class Channel;
class HttpInstance;

#define ILLSEQ 0
#define NEEDWT 1
#define SENTNE 2

/**
** Each user will be a subscriber
**/
class Subscriber : public Noncopyable
{
public:
    /// Constructor
    Subscriber(string sname, int seqid, string callback, Server *server, 
                Channel *channel, HttpInstance *instance);

    /// Destructor
    virtual ~Subscriber();

public:
    /// Send single message
    void   send(const string &message);
    
    /// Send multiple messages
    void   sendAllMessage();

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

    /// Each user will have a instance of HttpInstance
    HttpInstance *handler_;

    /// Each user will have a name(or id)
    string  sname_;

    /// Each user will keep a sequce_id(for message)
    int     seqid_;

    /// Each user will keep the name of callback in memory
    string  callback_;    
};

#endif