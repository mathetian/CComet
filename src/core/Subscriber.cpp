// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Channel.h"
#include "Subscriber.h"
#include "HttpInstance.h"

Subscriber::Subscriber(string sname, int seqid, string callback, 
                         Server *server, Channel *channel, HttpInstance *handler)
    : sname_(sname), seqid_(seqid), callback_(callback), 
        server_(server),  channel_(channel), handler_(handler)
{
    channel_ -> add(this);
    handler_ -> attachSubscriber(this);
}

Subscriber::~Subscriber()
{
    channel_ -> del(this);
}

void Subscriber::send(const string &message)
{
    handler_ -> write(concat(message));
}

void Subscriber::sendHistory()
{
    string message = channel_ -> getHistory();
    handler_ -> write(concat(message));
}

void Subscriber::check()
{
    if(seqid_ > channel_ -> getMxID())
    {
        /// TBD, write exceed information
        handler_ -> write("('[{\"type\" : \"403\"}]')");
    }

    if(seqid_ < channel_ -> getMxID())
    {
        string message = channel_ -> getSubHistory(seqid_);
        handler_ -> write(concat(message));
    }
}

void Subscriber::close()
{
    /// TBD
}

string Subscriber::getName() const
{
    return sname_;
}

string Subscriber::concat(const string &message)
{   
    return callback_ + "(\'[" + message + "]\')";
}