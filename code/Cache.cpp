//
// Created by Stephen Curry on 5/23/2023.
//

#include "Cache.h"
#include <thread>
#include "Epoller.h"
#include "TcpServer.h"
Cache* Cache::m_cache = new Cache;

Cache *Cache::getInstance() {
    return m_cache;
}

void Cache::deleteInstance() {
    if(m_cache)
    {
        delete m_cache;
        m_cache = nullptr;
    }
}

int Cache::close_fd(std::string uid)
{
    UID_map.erase(uid);
}

leave_user Cache::find_uid(std::string uid) {
    rwlock.rdlock(); // 读锁
    std::multimap<std::string, leave_user>::iterator it;
    it = UID_map.find(uid);
    rwlock.unlock(); // 释放读锁
    if (it == UID_map.end())
        return {0,0};
    else
        return UID_map[uid];
}

void Cache::time_out_exit(Log*log)
{
    while(isRun)
    {
        time_t now_time = time(nullptr); //当前时间
        std::multimap<std::string, leave_user>::iterator it; //迭代器
        rwlock.wrlock();
        for (it = UID_map.begin(); it != UID_map.end();)
        {
            if (now_time - it->second.time > MAXLEAVETIME) //超时退出
            {
                std::string info = it->first + "time out exit";
                std::cout << info << std::endl;
                log->Write(Log::info, info.data());
                m_closeConnectionCallback(it->second.tool); // 关闭连接
                it = UID_map.erase(it); // 更新迭代器，不要使用close_fd函数，因为会导致迭代器失效
            } else
            {
                it++;
            }
        }
        rwlock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(10)); //每10s检查一次
    }
}
