#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <map>
#include <list>
#include <vector>
#include <string>
using namespace std;

#include "Subscriber.h"

class Channel
{
public:
	Channel(string cname) : cname(cname) { }
	Subscriber *findSub(string name) 
	{
		if(subscribers.find(name) == subscribers.end())
			return NULL;
		return subscribers[name];
	}

	bool addSubscriber(Subscriber *suber)
	{
		string sname = suber->getSName();
		if(subscribers.find(sname) == subscribers.end())
			return false;
		subscribers[sname] = suber;
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
		msgs.push_back(msg); Subscriber *subscriber;
		map<string, Subscriber*>::iterator iter = subscribers.begin();
		for(;iter != subscribers.end();iter++)
		{
			subscriber = (*iter).second;
			subscriber->sendmsg(msg, msgs.size());
		}
	}

private:
	map<string, Subscriber*> subscribers;
	vector<string> msgs;
	string cname;
	string token;
};
#endif