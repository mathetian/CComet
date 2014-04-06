#include "Channel.h"
#include "Subscriber.h"
#include "HttpInstance.h"

Subscriber::Subscriber(string sname, Channel *channel, Server *server, HttpInstance *instance, int seqid)
    : sname(sname), channel(channel), server(server), instance(instance), seqid(seqid), callback("ccomet_cb")
{
    channel->addSubscriber(this);
    instance->addSubscriber(this);
}

void Subscriber::send(string msg)
{
    INFO << "Subscriber send: " << msg;
    msg = callback + "('[" + msg + "]')";
    INFO << "Subscriber send: " << msg;
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
    INFO << "Subscriber sendOldMsg: " << msg;
    instance->write(msg);
}

int Subscriber::trySend()
{
    if(channel->msgs.size() < seqid)
    {
        instance->write("Error SEQID");
        return ILLSEQ;
    }
    else if(channel->msgs.size() == seqid)
        return NEEDWT;
    else
    {
        string msg = callback + "('[";
        for(int i=seqid; i<channel->msgs.size(); i++)
        {
            if(i!=seqid) msg += ",";
            msg += channel->msgs[i];
        }
        msg += "]')";
        instance->write(msg);
        return SENTNE;
    }
}