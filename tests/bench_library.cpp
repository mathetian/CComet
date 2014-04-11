#include <vector>
using namespace std;

#include <sys/socket.h>
#include <sys/resource.h>

#include "../src/core/Socket.h"
#include "../src/core/EventLoop.h"
#include "../src/core/MsgHandler.h"
#include "../src/utils/TimeStamp.h"

int count, writes, fired;
int num_pipes, num_active, num_writes;

class EchoServer;

vector<Socket> pipes;
vector<EchoServer*> servers;

EventLoop loop;

class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop *loop, Socket sock, int idx) : MSGHandler(loop, sock, 1), idx(idx)
    { 
        printf("EchoServer %d %d\n",sock.get_fd(), idx);
    } 

    void clear()
    {
        m_status = 0; m_delflag = 0;
    }

private:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC)
        {
            INFO << "Received: " << (string)buf << " through fd " << m_sock.get_fd();
            count += buf.length();
            INFO << count ;
            if (writes > 0) 
            {
                int widx = idx + 1;
                if (widx >= num_pipes) widx -= num_pipes;
                ::write(pipes[2 * widx + 1].get_fd(), "m", 1);
                writes--;
                fired++;
            }

            if (fired == count) loop.stop(2);
        }
    }

    virtual void onCloseSocket(int st)
    {
        if(errno != 0) 
            DEBUG << strerror(errno);
        errno = 0;    
        DEBUG << "onCloseSocket: " << st << " " << m_sock.get_fd();
    }

private:
    int idx;
};

void run_once() 
{
    TimeStamp start = TimeStamp::now();

    int space = num_pipes / num_active;
    space *= 2;
    for (int i = 0; i < num_active; i++)
        ::write(pipes[i*space+1].get_fd(),"m",1);

    if(errno != 0) printf("error: %s\n",strerror(errno));
    
    fired  = num_active;
    count  = 0;
    writes = num_writes;

    TimeStamp before = TimeStamp::now();
    printf("%s\n", "runforever");
    loop.runforever();
    TimeStamp end = TimeStamp::now();

    fprintf(stdout, "%8ld %8ld\n", end.to_usecs() - before.to_usecs(), end.to_usecs() - start.to_usecs());
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


int main(int argc, char* argv[]) 
{
    num_pipes  = 100000;
    num_active = 1000;
    num_writes = num_pipes;

    setlimit(num_pipes);

    pipes = vector<Socket>(num_pipes*2,-1);

    for (int i = 0; i < num_pipes; i++) 
    {
        pair<Socket, Socket> ss = Socket::pipe();
        pipes[i*2]=ss.first;pipes[i*2+1]=ss.second;
    }

    for (int i = 0; i < num_pipes; i++) 
        servers.push_back(new EchoServer(&loop, pipes[i*2],i));

    for (int i = 0; i < 25; i++) 
    {
        run_once();
        fprintf(stdout, "writes=%d, fired=%d, recv=%d\n", writes, fired, count);
        for(int i=0;i<num_pipes;i++) servers[i]->clear();
    }
}
