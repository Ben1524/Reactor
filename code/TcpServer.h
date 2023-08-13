//
// Created by Stephen Curry on 5/9/2023.
//

#ifndef REACTOR_TCPSERVER_H
#define REACTOR_TCPSERVER_H

#include <netinet/in.h>
#include <thread>
#include "sys/socket.h"
#include "unistd.h"
#include "TcpTool.h"
#include "Epoller.h"
#include "memory"
#include "map"
#include "Cache.h"
#include "Log.h"
class TcpTool;
class Epoller;


class TcpServer {
public:
    TcpServer(int port_m):port(port_m),
                        listenFd(createListenFd(port_m)),
                        _listenTool(new TcpTool(listenFd)),
                        _epoll(new Epoller()){ // 监听epoll
        //非阻塞
        assert(listenFd >= 0); //监听套接字数量
        m_cache->setCloseConnectionCallback(std::bind(&TcpServer::_closeConnection,this,std::placeholders::_1));
        unique_ptr<std::thread> time_out_thread=make_unique<std::thread>(std::bind(&Cache::time_out_exit,m_cache,std::placeholders::_1),m_log);
        time_out_thread->detach();
    }
    ~TcpServer(){
        m_cache->isRun = false; //关闭超时检查线程
        close(listenFd);
    }
    void run(); //运行服务器

    int createListenFd(int port); //创建监听套接字
    void _closeConnection(TcpTool *tool);

private:

    void _acceptConnection();
    //处理读写
    void _handleWrite(TcpTool *tool);
    void _handleRead(TcpTool * tool);

    using EpollPtr = std::unique_ptr<Epoller>;
    using ListenTcpToolPtr = std::unique_ptr<TcpTool>;
private:
    int listenFd;
    int port;

    Cache* m_cache = Cache::getInstance();

    ListenTcpToolPtr _listenTool;   //
    EpollPtr _epoll;//只能有一个epoll实例，所以使用unique_ptr  自动释放

    Log* m_log = Log::GetInstance();

};


#endif //REACTOR_TCPSERVER_H
