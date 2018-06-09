#include "veClient.h"

// import extern function
extern void function_help(char *arg_value);

Session_Init::Session_Init()
{
    ret=-1;
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("socket error...\n");
        cout <<"socket ret: "<<sockfd<<endl;
        exit(ret);
    }

    //char *ip_address="192.168.1.103";
    int listen_port=atoi(G_Port);

    memset(&addr,0,sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_port=htons(listen_port);
    addr.sin_addr.s_addr=inet_addr(G_IP_Addr);
    //cout <<"服务器IP: "<<inet_ntoa(addr.sin_addr)<<" "<<"监听端口: "<<ntohs(addr.sin_port)<<endl;

    // true means socket_init success.
    init_flag=true;
}

Session_Init::~Session_Init()
{
    
}

int Session_Init::Get_Sockfd()
{
    return sockfd;
}

struct sockaddr_in Session_Init::Get_Addr()
{
    //cout <<"服务器IP: "<<inet_ntoa(addr.sin_addr)<<" "<<"监听端口: "<<ntohs(addr.sin_port)<<endl;
    return addr;
}

Chat_Session::Chat_Session()
{
    ret=-1;
}

Chat_Session::~Chat_Session()
{

}

void Chat_Session::Chat_Online(char *ptr_user_name)
{

    MSG msg;
    memset(&msg,0,sizeof(msg));
    msg.m_type=1;
    strncpy(msg.m_message,"login\n",MESSAGE_LENGTH-1);
    strncpy(msg.m_name,ptr_user_name,NAME_LENGTH-1);

    int sockfd=Get_Sockfd();
    struct sockaddr_in server_addr=Get_Addr();
    socklen_t addr_len=sizeof(server_addr);
    cout <<endl;
    cout <<"[服务器IP]: "<<inet_ntoa(server_addr.sin_addr);
    cout <<"                    ";
    cout <<"[通信端口]: "<<ntohs(server_addr.sin_port)<<endl<<endl;
    if(init_flag)
    {
        cout <<"[连接状态]: \033[32m"<<CONNECCT_STATUS_NORMAL<<"\033[0m";
    }
    else
    {
        cout <<"[连接状态]: \033[31m"<<CONNECCT_STATUS_ERROR<<"\033[0m";
    }
    cout <<"                           ";
    cout <<"[用户昵称]: "<<"\""<<msg.m_name<<"\""<<endl;
   
    cout <<endl;
    cout <<"**********************************************************************";
    cout <<endl;
    cout <<endl;
    cout <<"                          [消息公告区]                      "<<endl<<endl;
    //cout<<"第一次发送的m_type测试1: "<<msg.m_type<<endl<<endl;
    sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&server_addr,addr_len);
    //cout<<"第一次发送的IP: "<<inet_ntoa(server_addr.sin_addr)<<endl<<endl;
    //cout<<"第一次发送的m_type测试2: "<<msg.m_type<<endl<<endl;
    
    pid=fork();
    if(pid==0)
    {
        //cout<<"我是子进程"<<endl<<endl;
        //sub
        //cout<<"pid成功,进入sub..."<<endl;
        signal(SIGCHLD,SIG_IGN);
        while(fgets(msg.m_message,MESSAGE_LENGTH,stdin))
        {
            cout <<" "<<endl;
            if(strcmp(msg.m_message,"quit\n")==0)
            {
                msg.m_type=3;
                sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&server_addr,addr_len);
                //cout<<"消息广播的m_type: "<<msg.m_type<<endl;
                //cout<<"消息目的IP地址: "<<inet_ntoa(server_addr.sin_addr)<<endl<<endl;
                break;
            }

            msg.m_type=2;
            sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&server_addr,addr_len);
            //cout<<"消息广播的m_type: "<<msg.m_type<<endl;
            //cout<<"消息目的IP地址: "<<inet_ntoa(server_addr.sin_addr)<<endl<<endl;
        }

        close(sockfd);
        cout<<"程序将在5秒后退出..."<<endl<<endl;
        sleep(5);
        kill(getppid(),SIGKILL);
        exit(-1);
    }
    else if(pid>0)
    {
        //cout<<"我是父进程"<<endl;
        //parrent
        while(1)
        {
            //memset(&msg,0,sizeof(msg));
            recvfrom(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&server_addr,&addr_len);
            msg.m_name[NAME_LENGTH-1]='\0';
            msg.m_message[MESSAGE_LENGTH-1]='\0';
            switch(msg.m_type)
            {
                case 1:
                {
                    cout<<"\033[32m***系统提示***\033[0m: ["<<msg.m_name<<"] 上线了"<<endl<<endl;
                    break;
                }
                case 2:
                {
                    cout<<"[ \033[33m"<<msg.m_name<<"\033[0m] 说了: "<<msg.m_message<<endl;
                    break;
                }
                case 3:
                {
                    cout<<"\033[31m***系统提示***\033[0m: ["<<msg.m_name<<"] 下线了"<<endl<<endl;
                    break;
                }
                default:
                {
                    close(sockfd);
                    kill(pid,SIGKILL);
                    waitpid(pid,NULL,-1);
                }    
            }
        }
    }
}

int main(int argc,char **argv)
{

    if(argc<2)
    {
        cout<<endl;
        cout<<"\033[31mError:\033[0m  miss [name] segment."<<endl;
        cout<<endl;
        exit(-1);
    }

    // default value: ip:127.0.0.1 port:8888
    switch(argc)
    {
        case 2:
        {
            strncpy(G_User_Name,argv[1],NAME_LENGTH-1);
            strncpy(G_IP_Addr,"127.0.0.1",IP_LENGTH-1);
            strncpy(G_Port,"8888",PORT_LENGTH-1);
            break;
        }
        case 3:
        {
            strncpy(G_User_Name,argv[1],NAME_LENGTH-1);
            strncpy(G_IP_Addr,argv[2],IP_LENGTH-1);
            strncpy(G_Port,"8888",PORT_LENGTH-1);
            break;
        }
        case 4:
        {   
            strncpy(G_User_Name,argv[1],NAME_LENGTH-1);
            strncpy(G_IP_Addr,argv[2],IP_LENGTH-1);
            strncpy(G_Port,argv[3],PORT_LENGTH-1);
            break;
        }
    }

    Session_Init ss;

    Chat_Session cs;
    cs.Chat_Online(G_User_Name);

    return 0;
}