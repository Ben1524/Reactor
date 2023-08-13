//
// Created by Luo Ruiheng on 5/8/2023.
// Reactor
//

#ifndef REACTOR_TCPTOOL_H
#define REACTOR_TCPTOOL_H
#include "iostream"
#include "assert.h"
#include "unistd.h"
#include <cstring>
#include <vector>
#include "sys/socket.h"
#include "TcpSql.h"
#include "string"
#include "functional"
#include <netinet/in.h>
#include "sys/socket.h"
#include "Cache.h"
#include "Json_doc.h"
#include "Log.h"

#define BUFFMAXSIZE 1024

//==================handle==================================
//处理读写的class   具体的handle实例
class TcpTool {
public:
    explicit TcpTool(int fd):m_fd(fd){
        assert(m_fd>=0);
    }
    ~TcpTool(){
        close(m_fd);
    }

public:
    int getFd();
    int read();
    int sendn( const char *buffer, const size_t n,int fd);

    std::string Uid; // 用户id


    //@brief: 指定单个分隔符（单个字符或子串）分割字符串
    //@param: src 原字符串；delimiter 分隔符，单个字符或子串
    std::vector<std::string> splitStr(const std::string& src, const std::string& delimiter);

    void dowork(const char* buf);   // 处理读到的数据
    void enroll(const char* buf); // 注册
    void login(const char*); // 登录
    void transfer(const char*); // 转发消息
    void send_user();   // 发送在线用户
private:
    int m_fd;
    std::vector<std::string>vec_work;
    TcpSql* tcpsql = TcpSql::getInstance(); // 数据库操作

    Cache* cache = Cache::getInstance(); // 保存在线用户的fd,用于转发消息

    Log* m_log = Log::GetInstance(); // 日志

};



#endif //REACTOR_TCPTOOL_H
