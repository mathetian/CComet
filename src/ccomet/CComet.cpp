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

const string strs[3] = {"sign", "sub", "pub"};

bool isInSts(string str)
{
    int i;
    for(i=0; i<3&&str!=strs[i]; i++);
    if(i<3) return true;
    return false;
}

void HttpInstance::receivedMsg(STATUS status, Buffer &buf)
{
    parse(buf);
    INFO << "type: " << type;
    map<string,string>::iterator iter=keys.begin();
    for(; iter!=keys.end(); iter++)
        INFO << "Key: " << (*iter).first << " = " << (*iter).second;
    int flag = 0;
    if(errcode == 1 || !isInSts(type))
    {
        INFO << "Error HttpQuery, Errcode: " << errcode ;
        string msg = "ccomet_cb('{[\"type\" : \"404\"}]')";
        write(msg);
        
        clsStatus = 1;
        onCloseSocket(CLSHTP);
    }
    else
    {
        if(type=="sign") flag = server.sgn(keys, this);
        else if(type=="sub")  flag = server.sub(keys, this);
        else flag = server.pub(keys, this);

        if(flag != SUCCEEED && flag != NEEDCLSD)
        {
            INFO << "something error: " << flag;
            string msg = "ccomet_cb('[{\"type\" : \"401\"}]')";
            write(msg);
            
            clsStatus = 1;
            onCloseSocket(CLSHTP);
        }
        else if(flag == NEEDCLSD)
            onCloseSocket(CLSHTP);
    }
}

void HttpInstance::closedSocket()
{
    INFO << "HttpInstance closedSocket: " << clsStatus;
    if(clsStatus == 0) /**by peer**/
        subscriber->close();
    else /**by server**/ { }
}

int main()
{
    TCPAcceptor<HttpInstance> acceptor(loop, PORT);
    loop.runforever();

    return 0;
}