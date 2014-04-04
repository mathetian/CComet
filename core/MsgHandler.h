#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

#include <list>
using namespace std;

#include "SocketHandler.h"
#include "../utils/Buffer.h"
#include "../utils/Log.h"
using namespace utils;

#define MSGLEN 1024

class MSGHandler : public SocketHandler
{
public:
    MSGHandler(EventLoop& loop, Socket sock) : SocketHandler(loop)
    {
        m_sock = sock;
        attach(); registerRead();
    }

protected:
    int write(const Buffer& buf)
    {
        m_Bufs.push_back(buf);
        registerWrite();
    }

    int close()
    {
        onCloseSocket(CLSMAN);
    }

    typedef enum  { EXCEED, SOCKERR, BLOCKED, SUCC} STATUS;

private:
    virtual void onReceiveMsg()
    {
        Buffer buf(MSGLEN);
        int len = m_sock.read(buf.data(), MSGLEN);

        if(len == 0)  onCloseSocket(CLSEOF);
        else if(len < 0 && errno != EAGAIN) onCloseSocket(CLSERR);
        else if(len == MSGLEN) receivedMsg(EXCEED, buf);
        else if(len < 0) receivedMsg(BLOCKED, buf);
        else
        {
            buf.set_length(len);
            receivedMsg(SUCC, buf);
        }
    }

    virtual void onSendMsg()
    {
        while(m_Bufs.size() > 0)
        {
            Buffer buf = m_Bufs.front();
            m_Bufs.pop_front();

            const void *data = buf;
            uint32_t length  = buf.length();

            int len = m_sock.write(data, length);
            if(len < 0 && errno == EAGAIN)
            {
                m_Bufs.push_back(buf);
                registerWrite();
                break;
            }
            else if(len < 0) onCloseSocket(CLSWRR);
            else sendedMsg(SUCC, len, length);
        }
    }

    virtual void onCloseSocket(int st)
    {
        DEBUG << "onCloseSocket: " << st << " " << m_sock.get_fd();
        
        detach(); closedSocket();
        m_sock.close(); delete this;
    }

public:
    virtual void receivedMsg(STATUS status, Buffer &buf) { }
    virtual void sendedMsg(STATUS status, int len, int targetLen) { }
    virtual void closedSocket()              { }

private:
    list<Buffer>   m_Bufs;
};

#endif