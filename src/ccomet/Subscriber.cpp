#include "Channel.h"
#include "Subscriber.h"
#include "HttpInstance.h"

Subscriber::Subscriber(string sname, Channel *channel, Server *server,
                       HttpInstance *instance, int seqid, string callback)
    : sname(sname), channel(channel), server(server), instance(instance), seqid(seqid), callback(callback)
{
    channel->addSubscriber(this);
    instance->addSubscriber(this);
}

void Subscriber::send(string msg)
{
    msg = callback + "('[" + msg + "]')";
    DEBUG << "Subscriber send: " << msg;
    instance->write(msg);
}

void Subscriber::close(int type)
{
    instance->setStatus();
    if(type==0) channel->delSubscriber(this);
    delete this;
}

void Subscriber::sendOldMsg()
{
    string msg = callback + "('[";
    for(int i=0; i<channel->msgs.size(); i++)
    {
        if(i!=0) msg += ",";
        msg += channel->msgs[i];
    }
    msg += "]')";
    DEBUG << "Subscriber sendOldMsg: " << msg;
    instance->write(msg);
}

int Subscriber::trySend()
{
    if(channel->msgs.size() < seqid)
    {
        string msg = callback + "('[{\"type\" : \"402\"}]')";
        instance->write(msg);
        return ILLSEQ;
    }
    else if(channel->msgs.size() == seqid)
        return NEEDWT;
    else
    {
        string msg = callback + "('[" + channel->msgs[seqid];
        for(int i=seqid+1; i<channel->msgs.size(); i++)
            msg = msg + "," + channel->msgs[i];
        msg += "]')";
        instance->write(msg);
        return SENTNE;
    }
}