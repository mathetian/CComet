// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Server.h"
#include "HttpInstance.h"

HttpInstance::HttpInstance(EventLoop *loop, Socket sock) : \
    MSGHandler(loop, sock), errcode_(0), 
        clsStatus_(0), subscriber_(NULL), server_(Server::Instance())
{ 
}

HttpInstance::~HttpInstance()
{
}

void HttpInstance::receivedMsg(STATUS status, Buffer &receivedBuff)
{
    parse(receivedBuff);

    if(errcode_ == 1 || inSet(type) == false)
    {
        /// TBD 404 and Close The Socket        
    }
    else
    {
        STATUS flag;

        if(type=="sign") 
            flag = server_.sign(params_, this);
        else if(type=="sub")  
            flag = server_.subscriber(params_, this);
        else 
            flag = server_.publish(params_, this);

        /// TBD
    }
}
void HttpInstance::closedSocket()
{
   /// TBD
   /// Close By Peer
   /// OR
   /// Close By Manual
}

void HttpInstance::attachSubscriber(Subscriber *const subscriber)
{
    subscriber_ = subscriber;
}

void HttpInstance::parse(Buffer &buf)
{
    string str = buf;
    
    int i = 0, j;
    
    while(i < str.size() && str.at(i) == ' ') 
        i++;
    
    j = i;
    
    while(j < str.size() && str.at(j) != ' ') 
        j++;
    
    string msg = str.substr(i, j-i);

    if(j >= str.size() || msg != "GET")
        errcode_ = 1;
    else
    {
        i = j;
        while(i<str.size()&&str.at(i) == ' ') i++;
        j = i;
        while(j<str.size()&&str.at(j) != ' ') j++;

        if( j >= str.size() || str.at(i) != '/' || j == i + 1 )
            errcode_ = 1;
        else
        {
            msg = str.substr(i+1,j-i-1);
            parseSub(msg);
        }
    }
}

void HttpInstance::parseSub(const string &msg)
{
    int i = 0;
    while(i < msg.size() && msg.at(i) != '?') i++;
    
    if(i == 0 || i == msg.size())
        errcode_ = 1;
    else
    {
        type_ = msg.substr(0,i);
        parseParams(msg.substr(i+1));
    }
}

void HttpInstance::parseParams(const string &msg)
{
    int i = 0;
    while(i < msg.size() && errcode_ == 0)
    {
        int j = i;
        while(j < msg.size())
        {
            if(msg.at(j) != '&')
            {
                j++;
                continue;
            }
            else
            {
                if(j + 3 < msg.size())
                {
                    string st = msg.substr(j+1,3);
                    if(st=="gt;"||st=="lt;")
                    {
                        j++;
                        continue;
                    }
                }
                break;
            }
        }

        if(i==j)
        {
            errcode = 1;
            return;
        }
        
        parseKey(msg.substr(i,j-i));
        i=j+1;
    }
}

void HttpInstance::parseKey(const string &msg)
{
    int i = 0;

    while(i < msg.size() && msg.at(i) != '=') i++;
    
    if(i == 0||i == msg.size())
        errcode_ = 1;
    else
        params_[msg.substr(0,i)] = msg.substr(i+1);
}

bool HttpInstance::isInSets(string str)
{
    return (str == "sign" || str == "publish" || \
            str == "subscribe") ? true : false;
}