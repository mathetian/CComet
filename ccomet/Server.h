#ifndef _SERVER_H
#define _SERVER_H

#include <map>
#include <string>
using namespace std;

#include "Channel.h"
#include "Subscriber.h"
#include "HttpInstance.h"

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
		return new Channel(cname);
	}
	
private:
	map<string, Channel*> channels;
};
#endif