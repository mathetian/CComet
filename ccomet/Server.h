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
	int login(map<string, string> &keys, HttpInstance* const instance);
	int pub(map<string, string> &keys, HttpInstance* const instance);
	int sub(map<string, string> &keys, HttpInstance* const instance);

private:
	Channel* get_channel_by_id(const string &name)
	{
		if(cname_channels.find(name) == cname_channels.end())
			return NULL;
		return cname_channels[name];
	}

	Channel* new_channel(const string &cname)
	{
		return new Channel(cname);
	}
	
private:
	map<string, Channel*> cname_channels;
};
#endif