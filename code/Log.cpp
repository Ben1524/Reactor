#include "Log.h"

Log* Log::m_log = new Log(Log::file, Log::debug); //初始化静态成员变量，日志等级为debug，输出目标为文件

Log* Log::GetInstance() {
    return m_log;
}

void Log::deleteInstance() {
    if(m_log)
    {
        delete m_log;
        m_log = nullptr;
    }
}



Log::Log()
{
    this->target = terminal; //默认输出到终端
    this->level = debug; //默认日志等级为debug
    cout << "[WELCOME]" << Gettime().c_str()  << ":" << "START Logging" << endl;
}

Log::Log(log_target target, log_level level)
{
    this->target = target;
    this->level = level;
    this->path = Gettime();
    this->path +=".log";


    string tmp1 = ""; //输出到终端的欢迎语
    string welcome_dialog = tmp1 + "[WELCOME] "+ Gettime().c_str() + " : " + " START Logging ";

    //如果输入不是终端，则输入文件
    if (target != terminal)
    {
        this->outfile.open(path, ios::out | ios::app); //打开文件,以追加的方式写入
        this->outfile << welcome_dialog<<endl; //写入欢迎语

        this->outfile.close(); //关闭文件
    }
    //如果输入不是文件，则输入终端
    if (target != file)
    {
        cout << welcome_dialog;
    }
}

void Log::output(string text, log_level act_level) // 输出函数
{
    string prefix;
    if (act_level == debug) prefix = "[DEBUG]";
    else if (act_level == info) prefix = "[INFO]";
    else if (act_level == warning) prefix = "[WARNING]";
    else if (act_level == error) prefix = "[ERROR]";
    else prefix = "";
    prefix += " ";
    string output_content =  prefix+Gettime()+ " : " + text; //输出内容
    if (this->level <= act_level && this->target != file) {
        // 当前设定的等级才会显示在终端，且不能是只文件模式
        cout << output_content;
    }
    if (this->target != terminal)
    {
        this->outfile.open(this->path, ios::out | ios::app);
        this->outfile << output_content<<endl; //写入日志
        this->outfile.close();
    }
}

void Log::Write(log_level level,const char* text, ...)
{
    char buf[512] = { 0 };
    va_list ap; //定义一个va_list类型的变量，用来储存单个参数
    va_start(ap, text);//将ap指向text后的第一个参数，即可变参数列表的第一个参数
    vsprintf(buf, text, ap); //将参数写入buf,vprintf()函数用于将参数列表写入到第一个流中，第二个参数是格式化字符串，第三个参数是一个指向参数的指针
    va_end(ap);//将ap置为NULL
    output(buf, level); //输出
}

//获取当前系统时间
string Log::Gettime()
{
    //c++11获取系统时间
    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    // 将当前时间转换为time_t格式来进行输出
    time_t tt = std::chrono::system_clock::to_time_t(today); //将系统时间转换为time_t格式
    struct tm* ptm = localtime(&tt); //将time_t格式转换为tm结构体
    char date[60] = { 0 };
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday, (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return string(date);
}

void Log::Write(Log::log_level level, initializer_list<string> list)
{
    string output_content = "";
    for (auto it = list.begin(); it != list.end(); it++)
    {
        output_content += *it;
    }
    output(output_content, level);
}
