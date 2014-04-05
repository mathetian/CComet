#include "Channel.h"
#include "Subscriber.h"
#include "HttpInstance.h"

Subscriber::Subscriber(string sname, Channel *channel, Server *server, HttpInstance *instance, int seqid)
		: sname(sname), channel(channel), server(server), instance(instance), seqid(seqid) 
{ 
	channel->addSubscriber(this); 
}

void Subscriber::send(string msg)
{
	instance->write(msg);
}

void Subscriber::close()
{
	//Todo list
}