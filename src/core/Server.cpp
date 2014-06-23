// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Server.h"

int Server::sign(const Params &keys, const HttpInstance* const handler)
{
    //channel, sname
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end()\  
         || keys.find("callback") == keys.end())
        return ERRPARAM;

    string cname    = keys["channel"];
    string sname    = keys["sname"];
    string callback = keys["callback"];

    Channel     *channel = getChannel(cname);
    if(!channel) channel = newChannel(cname);

    Subscriber *subscriber = channel -> findSubscriber(sname);
    if(subscriber) return ERRDULPE;

    if(!subscriber) subscriber = new Subscriber(sname,channel,this,instance,0, callback);
    subscriber->sendOldMsg();

    string msg = channel -> formatStr(keys, "SIGN");
    channel -> send(msg);

    subscriber->close();
    return NEEDCLSD;
}

int Server::pub(const Params &keys, const HttpInstance* const handler)
{
    //channel, sname, msg
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end()
            || keys.find("msg") == keys.end() || keys.find("callback") == keys.end())
        return ERRPARAM;

    string cname = keys["channel"];
    string sname = keys["sname"];
    string msg   = keys["msg"];
    string callback = keys["callback"];

    Channel *channel = getChannel(cname);
    if(!channel) return ERRCHANL;

    string smsg = channel->formatStr(keys,"MSG");
    channel->send(smsg);

    msg = callback;
    msg += "('[{\"type\" : \"pub\"}]')";
    instance->write(msg);

    instance->setStatus();
    return NEEDCLSD;
}

int Server::sub(const Params &keys, const HttpInstance *const handler)
{
    //channel, sname, seqid
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end()
            || keys.find("seqid") == keys.end() || !is_int(keys["seqid"]) || keys.find("callback") == keys.end())
        return ERRPARAM;

    string channelId = keys["channel"];
    string sname     = keys["sname"];
    int seqid        = to_int(keys["seqid"]);
    string callback  = keys["callback"];

    Channel *channel = getChannel(channelId);
    if(!channel) return ERRCHANL;

    Subscriber *subscriber = channel->findSubscriber(sname);
    if(subscriber) return ERRDULPE;

    if(!subscriber) subscriber = new Subscriber(sname,channel,this,instance,seqid, callback);

    int flag = subscriber->trySend();
    switch(flag)
    {
    case ILLSEQ:
        subscriber->close();
        return NEEDCLSD;
    case SENTNE:
        subscriber->close();
        return NEEDCLSD;
    }

    return SUCCEEED;
}

Channel* Server::getChannel(const string &name)
{
    if(channels.find(name) == channels.end())
        return NULL;

    return channels[name];
}

Channel* Server::newChannel(const string &cname)
{
    Channel *channel= new Channel(cname);
    
    channels[cname] = channel;
    
    return channel;
}