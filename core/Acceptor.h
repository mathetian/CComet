#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "../utils/Log.h"
using namespace utils;

#include "SocketHandler.h"

template<class T>
class TCPAcceptor : public SocketHandler
{
public:
    TCPAcceptor(EventLoop& _loop, int localport) : SocketHandler(_loop)
    {
        NetAddress addr = NetAddress(localport);
        m_sock = TCPSocket(&addr);
        attach();
        registerRead();
        assert(m_sock.get_fd() >= 0);
        INFO << "TCPAcceptor Initialization" ;
        INFO << m_sock.getsockname() ;
    }

    TCPAcceptor(EventLoop& _loop, string ip, int localport) : SocketHandler(_loop)
    {
        NetAddress addr = NetAddress(ip,localport);
        m_sock = TCPSocket(&addr);
        attach();
        registerRead();
        assert(m_sock.get_fd() >= 0);
        INFO << "TCPAcceptor Initialization" ;
        INFO << m_sock.getsockname() ;
    }
    
private:
    void onReceiveMsg()
    {
        NetAddress a;
        TCPSocket sock = m_sock.accept(&a);
        INFO << "New Connection: " << sock.get_fd() << " " << sock.getpeername();
        if (sock.get_fd() >= 0) 
            T* t = new T(*getLoop(), sock);
    }

    void onSendMsg() { }
    void onCloseSocket(int st) 
    {
        assert(st == 0);
        DEBUG << "close listen socket fd: " << m_sock.get_fd();
        
        detach(); m_sock.close(); delete this;
    }
};

#endif