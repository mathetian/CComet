// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Server.h"

/// Singleton
Server& Server::Instance()
{
    static Server server_;
    return server_;
}

Server::Server()
{
}

Server::~Server()
{
}

STATUS1 Server::sign(Params &keys, HttpInstance* handler)
{
    /// channel, sname
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end() || keys.find("callback") == keys.end())
        return ERRPARAM;

    string cname    = keys["channel"];
    string sname    = keys["sname"];
    string callback = keys["callback"];

    Channel     *channel = getChannel(cname);
    if(!channel) channel = newChannel(cname);

    /// Find the subscriber
    Subscriber *subscriber = channel -> find(sname);
    if(subscriber) return ERRDULPE;

    /// Tell others in the same channel that somebody has logined in.
    string msg = channel -> format(keys, "SIGN");
    channel -> sendSign(msg);

    /// Create the subscriber
    if(!subscriber) subscriber = new Subscriber(sname, 0, callback, this, channel, handler);
    
    /// Send old message
    subscriber -> sendHistory();

    return SUCCEEED;
}

STATUS1 Server::publish(Params &keys, HttpInstance* handler)
{
    /// channel, sname, msg
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end()
            || keys.find("msg") == keys.end() || keys.find("callback") == keys.end())
        return ERRPARAM;

    string cname = keys["channel"];
    string sname = keys["sname"];
    string msg   = keys["msg"];
    string callback = keys["callback"];

    Channel *channel = getChannel(cname);
    if(!channel) return ERRCHANL;

    /// Find the subscriber
    Subscriber *subscriber = channel -> find(sname);

    /// Create the subscriber if not exist
    if(!subscriber) subscriber = new Subscriber(sname, 0, callback, this, channel, handler);

    string smsg = channel->format(keys, "MSG");
    channel -> sendChat(smsg);

    return SUCCEEED;
}

STATUS1 Server::subscribe(Params &keys, HttpInstance *handler)
{
    /// channel, sname, seqid
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end()
            || keys.find("seqid") == keys.end() || !is_int(keys["seqid"]) || keys.find("callback") == keys.end())
        return ERRPARAM;

    string channelId = keys["channel"];
    string sname     = keys["sname"];
    int seqid        = to_int(keys["seqid"]);
    string callback  = keys["callback"];

    Channel *channel = getChannel(channelId);
    if(!channel) return ERRCHANL;

    /// Find the subscriber
    Subscriber *subscriber = channel -> find(sname);

    /// Can't duplicate
    if(subscriber) return ERRDULPE;

    /// Create the subscriber if not exist
    if(!subscriber) subscriber = new Subscriber(sname, 0, callback, this, channel, handler);

    subscriber -> check();
    
    return SUCCEEED;
}

Channel* Server::getChannel(const string &name)
{
    if(channels_.find(name) == channels_.end())
        return NULL;

    return channels_[name];
}

Channel* Server::newChannel(const string &cname)
{
    Channel *channel= new Channel(cname);
    
    channels_[cname] = channel;
    
    return channel;
}