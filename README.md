



#single reactor + threadPool



# 流程图

```mermaid
flowchart  TB
subgraph 1
id1[TcpServe:负责新连接和关闭连接,管理所有</br>类,以及维护读写函数]
id2[TcpTool:每一个用户在登录成功都会创建,</br>负责消息的处理,以及对读写函数的具体实现]
id3[Log,TcpSql:分别负责日志记录和数据库处理]
id4[Cache:维护用户名UID到通信套接字,无操作</br>时间,TcpTool的映射]
id5[Epoller:epoll的封装,负责将事件分发以及</br>将回调函数交付给线程池处理]
end
subgraph 2
A("初始化TcpServer")-->B("单例模式")
B-->C("初始化缓存Cache")
B-->E("初始化日志Log")
A-->F("unique_ptr<>//只能有一个实例")
F-->G("ListenTcpToolPtr")
F-->H("EpollPtr _epoll;")
end
```

```mermaid
flowchart  TD
subgraph 3
A("TcpServer::run")--调用-->B("Epoller::wait")
B-->C(连接事件发生)--回调-->D(TcpServer::_acceptConnection)--创建-->K(为每个用户创建对应的</br>TcpTool)
B-->E(读事件发生)--线程池回调-->F(TcpServer::_handleRead)--调用-->G(每个用户对应的TcpTool对象)
B-->J(写事件发生--很少出现)--线程池回调-->M(TcpServer::_handleWrite)--调用-->G

N(子线程)--检查超时用户-->O(回调TcpServer::_closeConnection)
end
```







```c++
using NewConnectionCallback = std::function<void()>;  //相当于定义了一种函数类型
using HandleReadCallback = std::function<void(TcpTool*)>; // 处理读的回调用函数
using HandleWriteCallback = std::function<void(TcpTool*)>; // 处理写的回调函数
using CloseConnectionCallback = std::function<void(TcpTool*)>; // 关闭连接的回调函数

// this在这里使得回调函数是由当前TcpServer类来调用的
// std::bind 绑定成员函数要指定类的对象，因为成员函数是依赖于对象的
_epoll->setNewConnection(std::bind(&TcpServer::_acceptConnection,this));
// std::bind(&TcpServer::_acceptConnection,this)  返回一个可调用对象
_epoll->setWriteCb(std::bind(&TcpServer::_handleWrite,this,std::placeholders::_1));
// std::forward<decltype(PH1)>(PH1) 保持参数的类型 不会丢失右值的特性, 保持参数的完美转发
_epoll->setReadCb([this](auto && PH1) { _handleRead(std::forward<decltype(PH1)>(PH1)); });

```














