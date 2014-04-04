
#include "Server.h"

int Server::login(map<string, string> &keys, HttpInstance* const instance)
{
	//channel id, name
	if(keys.find("channelId") == keys.end() || keys.find("name") == keys.end())
		return 0;
	string channelId = keys["channelId"];
	string name      = keys["name"];
	Channel *channel = get_channel_by_id(channelId);
	if(!channel) channel = new_channel(channelId); 
	Subscriber *subscriber = channel->findSub(name);
	if(!subscriber) subscriber = new Subscriber(name,channel,this,instance);
	subscriber->start();
}

int Server::pub(map<string, string> &keys, HttpInstance* const instance)
{
	//channel id, name, msg
	if(keys.find("channelId") == keys.end() || keys.find("name") == keys.end()|| keys.find("msg") == keys.end())
		return 0;
	Channel *channel = get_channel_by_id(keys["channelId"]);
	if(!channel) return 0;
	channel->send(keys["msg"]);

	instance->closedSocket();
}

int Server::sub(map<string, string> &keys, HttpInstance *const instance)
{
	//channel id, name, seqid
	if(keys.find("channelId") == keys.end() || keys.find("name") == keys.end()|| keys.find("seqid") == keys.end())
		return 0;

	string channelId = keys["channelId"];
	string name      = keys["name"];
	string seqid     = keys["seqid"];

	Channel *channel = get_channel_by_id(channelId);
	if(!channel) return 0;
	
	Subscriber *subscriber = channel->findSub(name);
	if(!subscriber) subscriber = new Subscriber(name,channel,this,instance);
	subscriber->setseqid(seqid);	
	subscriber->process();
}

