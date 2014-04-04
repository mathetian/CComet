#ifndef _SUB_SCRIBER_H
#define _SUB_SCRIBER_H

#include <string>
using namespace std;

class Server;
class Channel;
class HttpInstance;

class Subscriber
{
public:
	Subscriber(string sname, Channel *channel, Server *server, HttpInstance *instance, int seqid)
		: sname(sname), channel(channel), server(server), instance(instance), seqid(seqid) { }
	void start();
	void process();
	string getSName();
	void sendmsg(string msg, int seqid);
private:
	Server *server;
	Channel *channel;
	HttpInstance *instance;
	int     msgseq;
	string  sname;
	int  seqid;
};
#endif