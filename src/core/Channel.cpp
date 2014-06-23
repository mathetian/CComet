// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "Header.h"
using namespace utils;

#include "Subscriber.h"
#include "HttpInstance.h"

Channel::Channel(string cname) : cname_(cname) 
{ 

}

Subscriber* Channel::find(const string &name)
{
    return subscribers.find(name) == subscribers.end() ?\
           NULL : subscribers[name];
}

bool Channel::add(Subscriber *subscriber)
{
    string name = subscriber -> getName();

    if(subscribers.find(name) != subscribers.end())
        return false;

    subscribers[name] = subscriber;
    return true;
}

bool Channel::del(Subscriber *suber)
{
    string name = subscriber -> getName();
    
    if(subscribers.find(sname) == subscribers.end())
        return false;
    
    subscribers.erase(sname);
    return true;
}

bool Channel::send(const string &msg)
{
    history_.push_back(msg);

    Subscriber *subscriber;
    
    map<string, Subscriber*>::iterator iter = subscribers.begin();
    
    for(; iter != subscribers.end(); iter++)
    {
        subscriber = (*iter).second;
        subscriber->send(msg);
        subscriber->close(1);
        subscriber->instance->close();
    }

    subscribers.clear();
}

string Channel::format(Params &keys, string type)
{
    string name   = keys["sname"], msg;
    string seq_id = to_string(msgs.size());
    
    if(type == "SIGN") 
        msg = "SIGN";
    else 
        msg = keys["msg"];

    string str = "{";
    str += " \"type\":\""    + type   + "\"" ;
    str += ",\"msg\":\""     + msg    + "\"" ;
    str += ",\"sname\":\""   + name   + "\"" ;
    str += ",\"seqid\":\""   + seq_id + "\"" ;
    str += ",\"channel\":\"" + cname  + "\"";
    str += "}";
    return str;
}