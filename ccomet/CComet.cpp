#include "../core/EventLoop.h"
#include "../core/MsgHandler.h"
#include "../core/Socket.h"
#include "../core/Acceptor.h"

#include "../utils/Log.h"
using namespace utils;

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

	if(errcode == 1)
	{
		INFO << "Error HttpQuery, Errcode: " << errcode ;
		write("Error HttpQuery");
		onCloseSocket(CLSHTP);
	}
	else if(type=="sign") server.sgn(keys, this);
	else if(type=="sub")  server.sub(keys, this);
	else if(type=="pub")  server.pub(keys, this);
	else
	{
		INFO << "Error HttpQuery" ;
		write("Error HttpQuery");
		onCloseSocket(CLSHTP);
	}
}

int main()
{
	TCPAcceptor<HttpInstance> acceptor(loop, PORT);
    loop.runforever();

	return 0;
}