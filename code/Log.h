#ifndef LOG_H
#define LOG_H
#pragma once
#include<unistd.h>
#include<iostream>
#include<fstream>
#include<string>
#include<chrono>
#include<initializer_list> //可变参数
#include<cstdarg> // 处理可变参数的宏
using namespace std;



class Log
{

public:
    static Log* GetInstance(void);
    static void deleteInstance();

    enum log_level { debug, info, warning, error };//日志等级
    enum log_target { file, terminal, file_terminal };//输出目标 文件，终端，文件和终端

//可变参数类型
    void Write(log_level level,const char* text, ...);
    void Write(log_level level,initializer_list<string> list);


    string Gettime();			//获取系统时间
private:
    std::ofstream outfile;		//输出文件
    log_level level;             //日志等级
    log_target target;          //日志输出目标
    string path;				//文件路径
    void output(string text, log_level act_level);//输出函数

private:
    Log();
    Log(log_target target, log_level level);
    ~Log() {deleteInstance();}
    static Log* m_log;

};

#endif
