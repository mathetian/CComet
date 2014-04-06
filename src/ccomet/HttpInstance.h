#ifndef _HTTP_INSTANCE_H
#define _HTTP_INSTANCE_H

#include <iostream>
using namespace std;

#include "../core/EventLoop.h"
#include "../core/MsgHandler.h"
#include "../core/Socket.h"
#include "../core/Acceptor.h"

#include "../utils/Log.h"
using namespace utils;

#include "Subscriber.h"

class HttpInstance :public MSGHandler
{
public:
    HttpInstance(EventLoop& loop, Socket sock) : MSGHandler(loop, sock), errcode(0), clsStatus(0) { }

public:
    virtual void receivedMsg(STATUS status, Buffer &buf);//in `ccomet.cpp`
    virtual void sendedMsg(STATUS status, int len, int targetLen) { }
    virtual void closedSocket();

    void    setStatus()
    {
        clsStatus = 1;
    }

    void    addSubscriber(Subscriber *const subscriber)
    {
        this->subscriber = subscriber;
    }
private:
    void parse(Buffer &buf)
    {
        string str = buf;
        int i=0,j;
        while(i<str.size()&&str.at(i) == ' ') i++;
        j=i;
        while(j<str.size()&&str.at(j) != ' ') j++;
        if(j>=str.size())
        {
            errcode = 1;
            return;
        }
        string msg = str.substr(i,j-i);
        if(msg!="GET")
        {
            errcode = 1;
            return;
        }
        i=j;
        while(i<str.size()&&str.at(i) == ' ') i++;
        j=i;
        while(j<str.size()&&str.at(j) != ' ') j++;

        if(j>=str.size() || str.at(i) != '/' || j == i+1)
        {
            errcode = 1;
            return;
        }
        msg = str.substr(i+1,j-i-1);
        parseSub(msg);
    }

    void parseSub(string msg)
    {
        int i = 0;
        while(i<msg.size() && msg.at(i) != '?') i++;
        if(i==0||i==msg.size())
        {
            errcode = 1;
            return;
        }
        type=msg.substr(0,i);
        parseParams(msg.substr(i+1));
    }

    void parseParams(string msg)
    {
        int i=0;
        while(i<msg.size()&&errcode == 0)
        {
            int j=i;
            while(j<msg.size())
            {
                if(msg.at(j)!='&') { j++; continue; }
                else
                {
                    if(j+3<msg.size())
                    {
                        string st = msg.substr(j+1,3);
                        if(st=="gt;"||st=="lt;")
                        { j++; continue; }
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

    void parseKey(string msg)
    {
        int i=0;
        while(i<msg.size() && msg.at(i) != '=') i++;
        if(i==0||i==msg.size())
        {
            errcode = 1;
            return;
        }
        keys[msg.substr(0,i)] = msg.substr(i+1);
    }

private:
    string type;
    int    errcode;
    map<string, string> keys;
    int    clsStatus;
    Subscriber *subscriber;
};

#endif