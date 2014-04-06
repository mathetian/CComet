#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <map>
#include <list>
#include <vector>
#include <string>
using namespace std;

#include "../utils/Utils.h"
#include "../utils/Log.h"
using namespace utils;

#include "Subscriber.h"
#include "HttpInstance.h"

class Channel
{
public:
    Channel(string cname) : cname(cname) { }

    Subscriber *findSubscriber(string name)
    {
        return subscribers.find(name) == subscribers.end() ?\
               NULL : subscribers[name];
    }

    bool addSubscriber(Subscriber *subscriber)
    {
        string sname = subscriber->getSName();
        if(subscribers.find(sname) != subscribers.end())
            return false;
        subscribers[sname] = subscriber;
        return true;
    }

    bool delSubscriber(Subscriber *suber)
    {
        string sname = suber->getSName();
        if(subscribers.find(sname) == subscribers.end())
            return false;
        subscribers.erase(sname);
        return true;
    }

    bool send(string msg)
    {
        msgs.push_back(msg);
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

    string formatStr(map<string, string> &keys, string type)
    {
        string sname = keys["sname"], msg;
        string seqid = to_string(msgs.size());
        if(type == "SIGN") msg = "SIGN";
        else msg = keys["msg"];

        string str = "{";
        str += " \"type\":\""    + type + "\"" ;
        str += ",\"msg\":\""     + msg + "\"" ;
        str += ",\"sname\":\""   + sname + "\"" ;
        str += ",\"seqid\":\""   + seqid + "\"" ;
        str += ",\"channel\":\"" + cname + "\"";
        str += "}";
        return str;
    }

private:
    map<string, Subscriber*> subscribers;
    vector<string> msgs;
    string cname;

    friend class Subscriber;
};
#endif