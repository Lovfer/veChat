#include "veClient.h"

// import extern function
extern void function_help(char *arg_value);

Session_Init::Session_Init()
{
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("socket error...");
        exit(ret);
    }

    char *ipAddress="192.168.1.103";

    memset(&addr,0,sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_port=htons(8888);
    addr.sin_addr.s_addr=inet_addr(ipAddress);

    //cout <<"AAAA服务器IP: "<<inet_ntoa(addr.sin_addr)<<" "<<"监听端口: "<<ntohs(addr.sin_port)<<endl;
}

// void Session_Init::Socket_Init()
// {
//     if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
//     {
//         perror("socket error...");
//         exit(ret);
//     }

//     char *ipAddress="127.0.0.1";

//     memset(&addr,0,sizeof(addr));

//     addr.sin_family=AF_INET;
//     addr.sin_port=htons(8888);
//     addr.sin_addr.s_addr=inet_addr(ipAddress);

//     cout <<"服务器IP: "<<inet_ntoa(addr.sin_addr)<<" "<<"监听端口: "<<ntohs(addr.sin_port)<<endl;

// }

int Session_Init::Get_Sockfd()
{
    return sockfd;
}

struct sockaddr_in Session_Init::Get_Addr()
{
    //cout <<"服务器IP: "<<inet_ntoa(addr.sin_addr)<<" "<<"监听端口: "<<ntohs(addr.sin_port)<<endl;
    return addr;
}

void Chat_Session::Chat_Online(char *user_name)
{

    MSG msg;
    memset(&msg,0,sizeof(msg));
    msg.m_type=1;
    strncpy(msg.m_message,"login",MESSAGE_LENGTH-1);
    strncpy(msg.m_name,user_name,NAME_LENGTH-1);

    int sockfd=Get_Sockfd();
    struct sockaddr_in server_addr=Get_Addr();
    socklen_t addr_len=sizeof(server_addr);
    cout <<"BBBB服务器IP: "<<inet_ntoa(server_addr.sin_addr)<<" "<<"监听端口: "<<ntohs(server_addr.sin_port)<<endl;
    cout <<"昵  称:"<<msg.m_name<<endl;
    cout <<"***********************************"<<endl;
    cout <<endl;
    cout<<"第一次发送的m_type测试1: "<<msg.m_type<<endl<<endl;
    sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&server_addr,addr_len);
    cout<<"第一次发送的IP: "<<inet_ntoa(server_addr.sin_addr)<<endl<<endl;
    cout<<"第一次发送的m_type测试2: "<<msg.m_type<<endl<<endl;
    
    pid=fork();
    if(pid==0)
    {
        cout<<"我是子进程"<<endl<<endl;
        //sub
        cout<<"pid成功,进入sub..."<<endl;
        signal(SIGCHLD,SIG_IGN);
        while(fgets(msg.m_message,MESSAGE_LENGTH,stdin))
        {
            if(strcmp(msg.m_message,"quit")==0)
            {
                msg.m_type=3;
                sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&server_addr,addr_len);
                cout<<"消息广播的m_type: "<<msg.m_type<<endl;
                cout<<"消息目的IP地址: "<<inet_ntoa(server_addr.sin_addr)<<endl<<endl;
                break;
            }

            msg.m_type=2;
            sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&server_addr,addr_len);
            cout<<"消息广播的m_type: "<<msg.m_type<<endl;
            cout<<"消息目的IP地址: "<<inet_ntoa(server_addr.sin_addr)<<endl<<endl;
        }

        close(sockfd);
        cout<<"程序将在5秒后退出..."<<endl;
        sleep(5);
        kill(getppid(),SIGKILL);
        exit(-1);
    }
    else if(pid>0)
    {
        cout<<"我是父进程"<<endl;
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
                    cout<<"系统提示: ["<<msg.m_name<<"] 上线了"<<endl<<endl;
                    break;
                }
                case 2:
                {
                    cout<<"["<<msg.m_name<<"] 说了: "<<msg.m_message<<endl<<endl;
                    break;
                }
                case 3:
                {
                    cout<<"系统提示: ["<<msg.m_name<<"] 下线了"<<endl<<endl;
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
    //function name
    int argv_len=strlen(*argv);
    char exec_name[argv_len];
    char *name_ptr;
    // //cout<<argv_len<<endl;
    // for(int i=0;i<1;i++)
    // {
    //     for(int j=2;j<argv_len;j++)
    //     {
    //         //cout<<argv[i][j];
    //         exec_name[j-2]=argv[i][j];
    //     }
    //     //cout<<endl;
    // }
    // name_ptr=exec_name;

    // if(argc<2)
    // {
    //     cout<<"miss user_name param, please view the example or help..."<<endl;
    //     function_help(argv);
    //     exit(-1);
    // }

    // if(argv[1]=="help")
    // {
    //     cout<<"[ "<<argv[1]<<" ]"<<"help: "<<endl;
    //     function_help(argv);
    //     exit(-1);
    // }

    if(argc<2)
    {
        cout<<"miss param name"<<endl;
        exit(_ERR_EXIT);
    }
        
    Session_Init ss;
    //ss.Socket_Init();

    Chat_Session cs;
    cs.Chat_Online(argv[1]);

    return 0;
}