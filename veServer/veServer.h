#ifndef _VESERVER_H_
#define _VESERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <malloc.h>

#include <iostream>
using namespace std;

#define MESSAGE_LENGTH 512
#define NAME_LENGTH 60
#define IP_LENGTH 32
#define PORT_LENGTH 16

#define CONNECCT_STATUS_NORMAL "normal"
#define CONNECCT_STATUS_ERROR  "error"

char G_IP_Addr[IP_LENGTH];
char G_Port[PORT_LENGTH];

// initial value=false
bool init_flag=false;

// msg_info
typedef struct msg_info
{
    int m_type;
    char m_name[NAME_LENGTH];
    char m_message[MESSAGE_LENGTH];
}MSG,*PMSG;

// user_list
typedef struct user_list
{
    struct sockaddr_in addr;
    struct user_list *next;
}USER_LIST,*PUSER_LIST;

// initial class session_init
class Session_Init
{
    public:
        Session_Init();
        ~Session_Init();
        int Get_Sockfd();
        struct sockaddr_in Get_Addr();
        
    private:
        int sockfd;
        int ret;
        struct sockaddr_in server_addr;
};

class Chat_Session:public Session_Init
{
    public:
        Chat_Session();
        ~Chat_Session();
        void Chat_Online();
        PUSER_LIST Add_Node(struct sockaddr_in *paddr);
        void Insert_Node(PUSER_LIST phead,struct sockaddr_in *paddr);
        void Destroy_Node(PUSER_LIST phead);
        void User_Login(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg);
        void Broadcast_Info(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg);
        void User_Quit(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg);
    private:
        int sockfd;
        int ret;
        struct sockaddr_in addr;
};

#endif // !_VESERVER_H_
