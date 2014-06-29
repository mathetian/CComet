// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Header.h"

#include "C.h"
using namespace utils;

#include "Channel.h"
#include "Subscriber.h"

namespace ccomet
{

Channel::Channel(string cname) : cname_(cname) 
{ 

}

Channel::~Channel()
{
}

Subscriber* Channel::find(const string &name)
{
    return subscribers_.find(name) == subscribers_.end() ?\
           NULL : subscribers_[name];
}

bool Channel::add(Subscriber *subscriber)
{
    string name = subscriber -> getName();

    if(subscribers_.find(name) != subscribers_.end())
        return false;

    subscribers_[name] = subscriber;
    return true;
}

bool Channel::del(Subscriber *subscriber)
{
    string name = subscriber -> getName();
    
    if(subscribers_.find(name) == subscribers_.end())
        return false;
    
    subscribers_.erase(name);
    return true;
}

string Channel::format(Params &keys, string type)
{
    string name   = keys["sname"], msg;
    string seq_id = to_string(history_.size());
    
    if(type == "SIGN") 
        msg = "SIGN";
    else 
        msg = keys["msg"];

    string str = "{";
    str += " \"type\":\""    + type   + "\"" ;
    str += ",\"msg\":\""     + msg    + "\"" ;
    str += ",\"sname\":\""   + name   + "\"" ;
    str += ",\"seqid\":\""   + seq_id + "\"" ;
    str += ",\"channel\":\"" + cname_  + "\"";
    str += "}";

    return str;
}

string Channel::getHistory() const
{
    string msg;

    for(int i = 0;i < history_.size();i++)
    {
        if(i != 0) msg += ",";
        msg += history_.at(i);
    }

    return msg;
}

string Channel::getSubHistory(int seqid) const
{
    string msg;

    for(int i = seqid;i < history_.size();i++)
    {
        if(i != seqid) msg += ",";
        msg += history_.at(i);
    }

    return msg;
}

int Channel::getMxID() const
{
    return history_.size();
}

void Channel::sendChat(const string &message)
{
    sendSign(message);
    history_.push_back(message);
}

void Channel::sendSign(const string &message)
{
    map<string, Subscriber*>::iterator iter = subscribers_.begin();
    
    for(;iter != subscribers_.end();iter++)
    {
        ((*iter).second) -> send(message);
    }

    subscribers_.clear();
}

};