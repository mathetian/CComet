
#include "Server.h"

int Server::sgn(map<string, string> &keys, HttpInstance* const instance)
{
	//channel, sname
	if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end())
		return 0;
	string cname = keys["channel"];
	string sname = keys["sname"];

	Channel     *channel = getChannel(cname);
	if(!channel) channel = newChannel(cname); 
	Subscriber *subscriber = channel->findSubscriber(sname);
	assert(subscriber == NULL);
	if(!subscriber) subscriber = new Subscriber(sname,channel,this,instance,0);
	
	channel->send(channel->formatStr(keys,"SIGN"));
	subscriber->close();
}

int Server::pub(map<string, string> &keys, HttpInstance* const instance)
{
	//channel, sname, msg
	if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end()|| keys.find("msg") == keys.end())
		return 0;

	string cname = keys["channel"];
	string sname = keys["sname"];
	string msg   = keys["msg"];

	Channel *channel = getChannel(cname);
	if(!channel) return 0;
	
	Subscriber *subscriber = channel->findSubscriber(sname);
	assert(subscriber == NULL);
	if(!subscriber) subscriber = new Subscriber(sname,channel,this,instance,0);

	channel->send(channel->formatStr(keys,"MSG"));
	subscriber->close();
}

int Server::sub(map<string, string> &keys, HttpInstance *const instance)
{
	//channel, sname, seqid
	if(keys.find("channelId") == keys.end() || keys.find("sname") == keys.end()
			|| keys.find("seqID") == keys.end() || !is_int(keys["seqID"]))
		return 0;
			
	string channelId = keys["channelId"];
	string sname     = keys["sname"];
	int seqID        = to_int(keys["seqID"]);

	Channel *channel = getChannel(channelId);
	if(!channel) return 0;
	
	Subscriber *subscriber = channel->findSubscriber(sname);
	if(!subscriber) subscriber = new Subscriber(sname,channel,this,instance,seqID);
}

