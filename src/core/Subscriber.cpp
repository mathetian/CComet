// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Server.h"
#include "Channel.h"
#include "Subscriber.h"

namespace ccomet
{

Subscriber::Subscriber(string sname, int seqid, string callback,
                       Server *server, Channel *channel, HttpRequest *req, HttpResponse *rep)
    : sname_(sname), seqid_(seqid), callback_(callback),
      server_(server),  channel_(channel), req_(req), rep_(rep)
{
    channel_ -> add(this);
}

Subscriber::~Subscriber()
{
    close();
}

void Subscriber::send(const string &message)
{
    rep_ -> addBody(concat(message));
    rep_ -> send();
}

bool Subscriber::check()
{
    rep_ -> addHeader("Content-Type", "text/javascript; charset=utf-8");

    if(seqid_ > channel_ -> getMxID())
    {
        channel_ -> del(this);

        rep_ -> addBody(concat("{\"type\" : \"403\"}"));
        rep_ -> send();

        return false;
    }
    else if(seqid_ < channel_ -> getMxID())
    {
        string message = channel_ -> getSubHistory(seqid_);

        rep_ -> addBody(concat("{\"type\" : \"403\"}"));
        rep_ -> send();

        return false;
    }

    return true;
}

void Subscriber::close()
{
    channel_ -> del(this);
}

string Subscriber::getName() const
{
    return sname_;
}

string Subscriber::concat(const string &message)
{
    return callback_ + "('[" + message + "]')";
}

};