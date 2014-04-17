#include "../core/EventLoop.h"
#include "../core/MsgHandler.h"
#include "../core/Socket.h"
#include "../core/Acceptor.h"

#include "../utils/Log.h"
using namespace utils;

#include <map>
using namespace std;

#include <sys/time.h>
#include <sys/resource.h>

#include "Server.h"
#include "HttpInstance.h"

#define BASE_PORT 10000
#define PORT_NUM  10

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
        if(keys.find("callback") != keys.end())
        {
            string callback = keys["callback"];
            string msg = callback + "('[{\"type\" : \"404\"}]')";
            write(msg);
        }
        clsStatus = 1;
        onCloseSocket(CLSHTP);
    }
    else
    {
        if(type=="sign") flag = server.sgn(keys, this);
        else if(type=="sub")  flag = server.sub(keys, this);
        else flag = server.pub(keys, this);

        if(flag == SUCCEEED) { }
        else if(flag == NEEDCLSD) onCloseSocket(CLSHTP);
        else
        {
            INFO << "something error: " << flag;
            if(flag == ERRPARAM || flag == ERRCHANL)
            {
                clsStatus = 1;
                onCloseSocket(CLSHTP);
            }
            else
            {
                string callback = keys["callback"];
                string msg = callback + "('[{\"type\" : \"401\"}]')";
                write(msg);

                onCloseSocket(CLSHTP);
            }
        }
    }
}

void HttpInstance::closedSocket()
{
    INFO << "HttpInstance closedSocket: " << clsStatus;
    if(clsStatus == 0 && subscriber) /**by peer**/
        subscriber->close();
    else /**by server**/ { }
}

int setlimit(int num_pipes)
{
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
    if (::setrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        fprintf(stderr, "setrlimit error: %s", strerror(errno));
        return 1;
    }
}

int main()
{
    setlimit(100000);

    vector<TCPAcceptor<HttpInstance>*> acceptors(PORT_NUM,NULL);

    for(int i = 0; i < PORT_NUM; i++)
        acceptors[i] = new TCPAcceptor<HttpInstance>(&loop, BASE_PORT+i);

    loop.runforever();

    for(int i = 0; i < PORT_NUM; i++)
    {
        delete acceptors[i];
        acceptors[i] = NULL;
    }

    return 0;
}