#include "../core/EventLoop.h"
#include "../core/MsgHandler.h"
#include "../core/Socket.h"
#include "../core/Acceptor.h"

#include <map>
using namespace std;

#include "Server.h"
#include "HttpInstance.h"

#define PORT 10000

EventLoop loop;
Server    server;

void HttpInstance::receivedMsg(STATUS status, Buffer &buf) 
{ 
	parse(buf);
	if(type=="login") server.login(keys, this);
	else if(type=="sub") server.sub(keys, this);
	else server.pub(keys, this);
}

int main()
{
	TCPAcceptor<HttpInstance> acceptor(loop, PORT);
    loop.runforever();

	return 0;
}