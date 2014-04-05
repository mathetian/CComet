#ifndef _SERVER_H
#define _SERVER_H

#include <map>
#include <string>
using namespace std;

#include "Channel.h"
#include "Subscriber.h"
#include "HttpInstance.h"

#define SUCCEEED 0
#define ERRPARAM 1
#define ERRCHANL 2
#define ERRDULPE 3
#define NEEDCLSD 4

class Server
{
public:
    int sgn(map<string, string> &keys, HttpInstance* const instance);
    int pub(map<string, string> &keys, HttpInstance* const instance);
    int sub(map<string, string> &keys, HttpInstance* const instance);

private:
    Channel* getChannel(const string &name)
    {
        if(channels.find(name) == channels.end())
            return NULL;
        return channels[name];
    }

    Channel* newChannel(const string &cname)
    {
        Channel *channel= new Channel(cname);
        channels[cname] = channel;
        return channel;
    }

private:
    map<string, Channel*> channels;
};
#endif