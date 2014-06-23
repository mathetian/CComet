// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Channel.h"
#include "Subscriber.h"
#include "HttpInstance.h"

Subscriber::Subscriber(string sname, int seqid, string callback, 
                         Server *server, Channel *channel, HttpInstance *handler))
    : sname_(sname), handler_(handler), server_(server), 
        instance_(instance), seqid_(seqid), callback_(callback)
{
    channel_ -> addSubscriber(this);
    handler_ -> addSubscriber(this);
}

Subscriber::~Subscriber()
{
    
}

void Subscriber::send(const string &message)
{
    handler_ -> write(concat(message));
}

void Subscriber::close()
{
    /// TBD
}

string Subscriber::getName() const
{
    return sname_;
}

void Subscriber::sendAllMessage()
{
    string message = channel -> getAllMessage();
    handler_ -> write(concat(message));
}

string Subscriber::concat(const string &message)
{   
    return callback + "('[" + message + "]')";
}