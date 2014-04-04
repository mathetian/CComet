#ifndef _HTTP_INSTANCE_H
#define _HTTP_INSTANCE_H

#include "../core/EventLoop.h"
#include "../core/MsgHandler.h"
#include "../core/Socket.h"
#include "../core/Acceptor.h"

class HttpInstance :public MSGHandler
{
public:
	HttpInstance(EventLoop& loop, Socket sock) : MSGHandler(loop, sock) { }

public:
	virtual void receivedMsg(STATUS status, Buffer &buf);
    virtual void sendedMsg(STATUS status, int len, int targetLen) { }
    virtual void closedSocket();

private:
	void parse(Buffer &buf) { }

private:
	string type;
	map<string, string> keys;
};

#endif