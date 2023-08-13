//
// Created by Stephen Curry on 5/23/2023.
// 缓存在线的用户,可以转发消息
//

#ifndef REACTOR_CACHE_H
#define REACTOR_CACHE_H
#include "map"
#include "iostream"
#include "Log.h"
#include <functional>
// 添加读写锁
#include <pthread.h>
#define MAXLEAVETIME 180 //3min

class TcpServer;
class TcpTool;

struct leave_user{
    int fd; //套接字
    time_t time; //时间戳
    TcpTool*tool;// 退出时使用
};
class Cache {
public:

    using CloseConnectionCallback = std::function<void(TcpTool*)>; // 关闭连接的回调函数

    static void deleteInstance();

    static Cache* getInstance(void);


    /*第二个int 改成结构体*/
    /*
     * int fd
     * time 时间戳 判断是否过期
     * */
    std::map<std::string,leave_user>UID_map; // 保存在线用户的fd,用于转发消息

    void bianli()
    {
        for(auto iter:UID_map)
        {
            std::cout<<iter.first<<"  "<<iter.second.fd<<"leave time:"<<iter.second.time<<std::endl;
        }
    }

    /*添加定时器,当3min后 删除不活跃的用户缓存*/

    int close_fd(std::string);
    // 超时退出
    bool isRun = true;
    void time_out_exit(Log*log);
    void setCloseConnectionCallback(const CloseConnectionCallback& cb)
    {
        m_closeConnectionCallback = cb;
    }
    void updateTime(const std::string& uid)
    {
        rwlock.wrlock();
        UID_map[uid].time = time(nullptr);
        rwlock.unlock();
    }
    leave_user find_uid(std::string);

private:

    Cache(){
        ptread_rwlock_init(&rwlock,nullptr);
    };
    ~Cache(){
        ptread_rwlock_destroy(&rwlock);

    };
    Cache(const Cache& cache) = delete;
    Cache& operator=(const Cache& cache) = delete;

private:
    static Cache* m_cache;
    CloseConnectionCallback m_closeConnectionCallback;
    pthread_rwlock_t rwlock;
};


#endif //REACTOR_CACHE_H
