//
// Created by Stephen Curry on 5/8/2023.
//


#include "TcpTool.h"

int TcpTool::read() {
    char buf[BUFFMAXSIZE];
    memset(buf,0,sizeof(buf));
    int ret = recv(m_fd,buf,sizeof(buf),0);
    if(ret == 0)
    {
        std::cout<<"link error"<<std::endl;
        m_log->Write(Log::info,"link error");

    }
    else if(ret >0)
    {
        std::cout<<"recv:"<<buf<<std::endl;
        m_log->Write(Log::info,"recv:%s",buf);
        dowork(buf); //处理消息
    }
    else if(ret == -1)
    {
        std::cout<<"recv error!"<<std::endl;
        std::cout<<strerror(errno)<<std::endl; //打印错误信息，strerror将错误码转换成错误信息
    }

    return ret;
}
int TcpTool::sendn(const char *buffer, const size_t n, int fd){
    int remain, len, idx;
    remain = (n == 0) ? strlen(buffer) : n; // 判断发送长度正不正确
    idx = 0; // 已发送的长度
    while (remain > 0) { // 循环发送，直到发送完毕，避免消息过长，一次发送不完
        if ((len = send(fd,buffer + idx, remain,0)) <= 0) // 发送失败
            return 0;
        idx += len; // 已发送的长度
        remain -= len; // 剩余的长度
    }
    m_log->Write(Log::info,"send %s to traget_fd %d",buffer,fd);
    return 1;
}

int TcpTool::getFd() {
    return m_fd;
}

void TcpTool::dowork(const char *buf) { //处理消息
    auto vac_work = splitStr(buf,"##");  // 拆分消息

    //加入防伪，防止被直接连接
    if(vac_work[0] == "mes") //判断消息是否正确，mes为消息头
    {
        switch (atoi(vac_work[1].data())) { //判断消息类型
            case 1://注册
            {
                m_log->Write(Log::debug,"user enroll");
                enroll(vac_work[2].data()); //处理注册消息
                break;
            }

            case 2://登录
            {
                m_log->Write(Log::debug,"user login"); //处理登录消息
                login(vac_work[2].data()); //处理登录消息
                break;
            }
            case 3://QQDAY##3##UID@@info
                transfer(vac_work[2].data()); //处理转发消息
                break;
            case 4://客户端 get当前在线的用户
                send_user(); //发送当前在线的用户
                break;
            default:
                break;
        }
    }
    else
    {
        sendn("erron",0,this->m_fd);
    }
}

void TcpTool::enroll(const char buf[BUFFMAXSIZE]) {// 处理注册消息,注册之后进行登录
    auto info = splitStr(buf,"@@"); //拆分消息,消息格式为 mes##1##UID@@info
    if(info.size()>1)
    {
        if (tcpsql->Add(info[0], info[1]) == 1)//成功
        {
            sendn("enroll##true",0,this->m_fd);
        }
        else{
            sendn("enroll##false",0,this->m_fd);
        }
    }
}

void TcpTool::login(const char buf[BUFFMAXSIZE])
{
    auto info = splitStr(buf,"@@");
    if(info.size()>1)
    {
        if (tcpsql->Select(info[0], info[1]) == 1)//成功
        {
            /*存放UID 和 fd和离开时间*/
            leave_user user;
            user.fd = this->m_fd;
            user.time = time(NULL); //获取当前时间
            user.tool= this; // 记录
            cache->UID_map.insert(std::pair(info[0],user));

            this->Uid = info[0];

            cache->bianli(); //遍历map,输出当前在线的用户
            sendn("login##true",0,this->m_fd); //发送登录成功消息
            /* 1.发送在线用户给客户
             * 2.数据格式 JSON
             * 3.用户自己的数据
             * */
        }
        else {
            sendn("login##false",0,this->m_fd);
        }
    }
}

//转发消息
/*QQDAY##3##UID@@msg*/
void TcpTool::transfer(const char buf[BUFFMAXSIZE])
{
    auto info = splitStr(buf,"@@");
    string message = "MESSAGE##"+info[1];
    int target_fd = cache->find_uid(info[0]).fd;
    if(target_fd!=0)
        sendn(message.data(),0,target_fd);
    else
        std::cout<<"target_fd错误\n";
}

//发送在线用户给客户端
void TcpTool::send_user() {
    JSON json;
    vector<string> hobbies;
    for(auto iter:cache->UID_map)
    {
        hobbies.push_back(iter.first);
    }
    json.addArray("hobbies", hobbies);// 添加json数组
    string users_info = "json##"+json.pack();
    for(auto iter:cache->UID_map)
    {
        sendn(users_info.data(),0,iter.second.fd); //发送给每个用户
    }
}

std::vector<std::string> TcpTool::splitStr(const std::string& src, const std::string& delimiter) {
    std::vector<std::string> vetStr;

    // 入参检查
    // 1.原字符串为空或等于分隔符，返回空 vector
    if (src == "" || src == delimiter) {
        return vetStr;
    }
    // 2.分隔符为空返回单个元素为原字符串的 vector
    if (delimiter == "") {
        vetStr.push_back(src);
        return vetStr;
    }

    std::string::size_type startPos = 0;
    auto index = src.find(delimiter); // 查找第一个分隔符对应的下标，返回值为 std::string::size_type 类型
    while (index != std::string::npos) { // 对应的迭代器不是末尾
        auto str = src.substr(startPos, index - startPos);
        if (str != "") {
            vetStr.push_back(str);
        }
        startPos = index + delimiter.length();
        index = src.find(delimiter, startPos);
    }
    // 取最后一个子串
    auto str = src.substr(startPos);
    if (str != "") {
        vetStr.push_back(str);
    }

    return vetStr;
}


