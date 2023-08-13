#include "iostream"
#include "TcpServer.h"
int main(int argc ,char ** argv)
{
    int port = 10000;
    if(argc >= 2){
        port = atoi(argv[1]);
    }

    TcpServer serv(port);
    serv.run();

    return 0;

}