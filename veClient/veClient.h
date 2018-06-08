#ifndef _VICLIENT_H_
#define _VICLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <iostream>
using namespace std;

#define NAME_LENGTH 32
#define MESSAGE_LENGTH 512

// ERR_CODE
#define _ERR_EXIT -1

// function_help
void function_help(char *arg_value)
{
    cout<<"  eg_01: \"./"<<*arg_value<<" user_name\""<<endl;
    cout<<"  eg_02: \"./"<<*arg_value<<" help\""<<endl;
    cout<<endl;
}

// msg_info
typedef struct msg_info
{
    int m_type;
    char m_name[NAME_LENGTH];
    char m_message[MESSAGE_LENGTH];
}MSG,*PMSG;

void check(char *option_name,int fd)
{
    if(fd<0)
        cout<<*option_name<<" error..."<<endl;
}


class Session_Init
{
    public:
        Session_Init();
        //void Socket_Init();
        int Get_Sockfd();
        struct sockaddr_in Get_Addr();
    private:
        int sockfd;
        int ret;
        struct sockaddr_in addr;
};

class Chat_Session:public Session_Init
{
    public:
        //Chat_Session(){};
        void Chat_Online(char *user_name);
    private:
        pid_t pid;
};

#endif // !_VICLIENT_H_
