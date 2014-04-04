#ifndef _EVENT_LOOP_H
#define _EVENT_LOOP_H

#include <map>
using namespace std;

#include "SocketHandler.h"
#include "Acceptor.h"
#include "Selector.h"

class EventLoop
{   
    map<int, SocketHandler*> m_map;
    bool   m_quitFlag;
    Selector *m_selector;

public:
    EventLoop() : m_quitFlag(false), m_selector(new Selector(this))
    { }

    ~EventLoop()
    {
        map<int, SocketHandler*>::iterator iter = m_map.begin();
        for(; iter != m_map.end(); iter++)
        {
            SocketHandler *tmpHandler = iter->second;
            delete tmpHandler; tmpHandler = NULL;
        }
        m_map.clear();
        delete m_selector; m_selector = NULL;
    }

    void runforever()
    {
        while (!m_quitFlag)
            run_once();
        map<int,SocketHandler*>::iterator iter = m_map.begin();
        for(;iter!=m_map.end();iter++)
        {
            SocketHandler *handler = (*iter).second;
            m_selector->unRegisterEvent(handler, -1);
            handler->onCloseSocket(CLSSIG);
        }
    }

    void run_once()
    {
        m_selector->dispatch();
    }

    void stop()
    {
        m_quitFlag = 1;
    }
    
    void attachHandler(int fd, SocketHandler *p)
    {
        assert(m_map.find(fd) == m_map.end());
        m_map[fd] = p;
    }

    void detachHandler(int fd, SocketHandler *p)
    {
        assert(m_map.find(fd) != m_map.end());
        assert(m_map[fd] == p);
        assert(m_map.erase(fd) == 1);
        m_selector->unRegisterEvent(p, -1);
    }

private:
    void registerRead(int fd)
    {
        assert(m_map.find(fd) != m_map.end());
        m_selector->registerEvent(m_map[fd], EPOLLIN);
    }

    void registerWrite(int fd)
    {
        assert(m_map.find(fd) != m_map.end());
        m_selector->registerEvent(m_map[fd], EPOLLOUT);
    }

    void unRegisterRead(int fd)
    {
        assert(m_map.find(fd) != m_map.end());
        m_selector->unRegisterEvent(m_map[fd], EPOLLIN);
    }

    void unRegisterWrite(int fd)
    {
        assert(m_map.find(fd) != m_map.end());
        m_selector->unRegisterEvent(m_map[fd], EPOLLOUT);
    }

    friend class SocketHandler;

    template<class T> friend  class TCPAcceptor;

public:
    void addActive(int fd,int type)
    {
        assert(m_map.find(fd) != m_map.end());
        SocketHandler *handler = m_map[fd];

        if((type & EPOLLIN) != 0)
            handler->onReceiveMsg();

        if((type & EPOLLOUT) != 0)
        {
            m_selector->unRegisterEvent(handler, EPOLLOUT);
            handler->onSendMsg();
        }

        if((type & EPOLLRDHUP) || (type & EPOLLERR) || (type & EPOLLHUP))
        {
            m_selector->unRegisterEvent(handler, -1);
            handler->onCloseSocket(CLSEVT);
        }
    }
};

inline void SocketHandler::attach()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->attachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::detach()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->detachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::registerRead()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->registerRead(m_sock.get_fd());
}

inline void SocketHandler::registerWrite()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->registerWrite(m_sock.get_fd());
}

inline void SocketHandler::unRegisterRead()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->unRegisterRead(m_sock.get_fd());
}

inline void SocketHandler::unRegisterWrite()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->unRegisterWrite(m_sock.get_fd());
}

inline int Selector::dispatch()
{
    int num;

    int timeout = 5*1000; //5s

    num = epoll_wait(m_epollfd, m_events, MAX_NEVENTS, timeout);
    for(int i = 0; i < num; i++)
    {
        int what = m_events[i].events;
        SocketHandler *handler = (SocketHandler*)m_events[i].data.ptr;
        m_loop->addActive(handler->getSocket().get_fd(), what);
    }
}

#endif