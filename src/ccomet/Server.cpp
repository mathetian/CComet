
#include "Server.h"

int Server::sgn(map<string, string> &keys, HttpInstance* const instance)
{
    //channel, sname
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end())
        return ERRPARAM;

    string cname = keys["channel"];
    string sname = keys["sname"];

    Channel     *channel = getChannel(cname);
    if(!channel) channel = newChannel(cname);
    string msg = channel->formatStr(keys,"SIGN");
    channel->send(msg);

    Subscriber *subscriber = channel->findSubscriber(sname);
    if(subscriber) return ERRDULPE;
    if(!subscriber) subscriber = new Subscriber(sname,channel,this,instance,0);
    subscriber->sendOldMsg();

    subscriber->close();
    return NEEDCLSD;
}

int Server::pub(map<string, string> &keys, HttpInstance* const instance)
{
    //channel, sname, msg
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end()|| keys.find("msg") == keys.end())
        return ERRPARAM;

    string cname = keys["channel"];
    string sname = keys["sname"];
    string msg   = keys["msg"];

    Channel *channel = getChannel(cname);
    if(!channel) return ERRCHANL;

    string smsg = channel->formatStr(keys,"MSG");
    channel->send(smsg);
    instance->write(smsg);

    instance->setStatus();
    return NEEDCLSD;
}

int Server::sub(map<string, string> &keys, HttpInstance *const instance)
{
    //channel, sname, seqid
    if(keys.find("channel") == keys.end() || keys.find("sname") == keys.end()
            || keys.find("seqid") == keys.end() || !is_int(keys["seqid"]))
        return ERRPARAM;

    string channelId = keys["channel"];
    string sname     = keys["sname"];
    int seqid        = to_int(keys["seqid"]);

    Channel *channel = getChannel(channelId);
    if(!channel) return ERRCHANL;

    Subscriber *subscriber = channel->findSubscriber(sname);
    if(subscriber) return ERRDULPE;

    if(!subscriber) subscriber = new Subscriber(sname,channel,this,instance,seqid);

    int flag = subscriber->trySend();
    switch(flag)
    {
    case ILLSEQ:
        subscriber->close();
        return NEEDCLSD;
    case SENTNE:
        subscriber->close();
        return NEEDCLSD;
    }

    return SUCCEEED;
}