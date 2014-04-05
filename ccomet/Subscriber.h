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
	Subscriber(string sname, Channel *channel, 
		Server *server, HttpInstance *instance, int seqid);

public:
	string getSName() { return sname; }
	void   send(string msg);
	void   close();

private:
	Server  *server;
	Channel *channel;
	HttpInstance *instance;
	string  sname;
	int     seqid;
};

#endif