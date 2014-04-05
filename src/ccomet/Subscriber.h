#ifndef _SUB_SCRIBER_H
#define _SUB_SCRIBER_H

#include <string>
using namespace std;

class Server;
class Channel;
class HttpInstance;

#define ILLSEQ 0
#define NEEDWT 1
#define SENTNE 2

class Subscriber
{
public:
    Subscriber(string sname, Channel *channel,
               Server *server, HttpInstance *instance, int seqid);

public:
    string getSName()
    {
        return sname;
    }
    void   send(string msg);
    void   close(int type=0);
    void   sendOldMsg();
    int    trySend();

private:
    Server  *server;
    Channel *channel;
    HttpInstance *instance;
    string  sname;
    int     seqid;

    friend class Channel;
};

#endif