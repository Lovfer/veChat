#include "viServer.h"

PUSER_LIST Chat_Session::Add_Node(struct sockaddr_in *paddr)
{
    cout<<"Add_Node_ADDR: "<<inet_ntoa(paddr->sin_addr)<<endl;
    PUSER_LIST node=(PUSER_LIST)malloc(sizeof(USER_LIST));    
    if(NULL == node)
        cout<<"malloc error!"<<endl;
    node->addr = *paddr;
    return node;
}

void Chat_Session::Insert_Node(PUSER_LIST phead,struct sockaddr_in *paddr)
{
    cout<<"InsertLogin_IP_ADDR: "<<inet_ntoa(paddr->sin_addr)<<endl;
    PUSER_LIST node = Add_Node(paddr);
    node->next =phead->next;
    phead->next = node;    
}

void Chat_Session::Destroy_Node(PUSER_LIST phead)
{
    PUSER_LIST head;
    head=phead;  
    while(head)
    {
        PUSER_LIST tmp_node = head->next;
        free(head);
        head = tmp_node;
    }
    phead = NULL;
}

void Chat_Session::User_Login(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg)
{
    cout<<"+++++++++++++++++user_login++++++++++++++++++++++++++++++++"<<endl;
    Insert_Node(phead, paddr);
    cout<<"插入成功"<<endl<<endl;
    cout<<"Insert_Node后,phead指向的地址信息: "<<phead<<endl;
    cout<<"Insert_Node后,phead存储的地址信息: "<<inet_ntoa(phead->addr.sin_addr)<<endl;
    cout<<"Insert_Node后,phead指向的next信息: "<<phead->next<<endl;
    PUSER_LIST ptr=phead;
    phead = phead->next;
    cout<<"phead = phead->next后,phead指向的地址信息: "<<phead<<endl;
    cout<<"phead = phead->next后,phead存储的地址信息: "<<inet_ntoa(phead->addr.sin_addr)<<endl;
    cout<<"phead = phead->next后,phead指向的next信息: "<<phead->next<<endl;
    cout<<"@@@@@@@@@@@@@@@@@@@@@消息读取完毕@@@@@@@@@@@@@@@@@@@@@"<<endl<<endl;
    //每次新增加结点之后,phead总是指向尾结点，新增的结点
    // 从此之后才为以前的链表
    while(ptr->next)
    {
        //phead = phead->next;
        ptr=ptr->next;
        sendto(sockfd, msg, sizeof(*msg), 0, (struct sockaddr*)&phead->addr, sizeof(struct sockaddr_in));
    }
}

void Chat_Session::Broadcast_Info(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg)
{
    cout<<"**************Broadcase_info********************"<<endl;
    cout<<"+++++++++++++++++Broadcase_info++++++++++++++++++++++++++++++++"<<endl;
    int flag = 0; //1表示已经找到了
    cout<<endl;
    cout<<"paddr传入地址外层测试: "<<inet_ntoa(paddr->sin_addr)<<endl;
    while(phead->next)
    {
        phead = phead->next;
        cout<<"phead广播地址测试: "<<inet_ntoa(phead->addr.sin_addr)<<endl;
        cout<<"paddr传入地址内层测试: "<<inet_ntoa(paddr->sin_addr)<<endl;
        if((flag) || !(flag=memcmp(paddr, &phead->addr, sizeof(struct sockaddr_in))==0))
        {
            cout<<inet_ntoa(phead->addr.sin_addr)<<endl;
            sendto(sockfd, msg, sizeof(*msg), 0, (struct sockaddr*)&phead->addr, sizeof(struct sockaddr_in));
        }
    }
    cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl<<endl;
}

void Chat_Session::User_Quit(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg)
{
    cout<<"User_Quit"<<endl;
    int flag = 0;//1表示已经找到
    while(phead->next)
    {
        if((flag) || !(flag = memcmp(paddr, &phead->next->addr, sizeof(struct sockaddr_in))==0))
        {
            sendto(sockfd, msg, sizeof(*msg), 0, (struct sockaddr*)&phead->next->addr, sizeof(struct sockaddr_in));
            phead = phead->next;
        }        
        else
        { //删除这个退出的用户
            PUSER_LIST tmp_node = phead->next;
            phead->next = tmp_node->next;
            free(tmp_node);
        }
    }
    cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl<<endl;        
}

Session_Init::Session_Init()
{
    //create socket
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("socket error...");
        exit(ret);
    }

    //char *ipAddress="127.0.0.1";
    int on = 1; 
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ); 

    //bind socket
    memset(&server_addr,0,sizeof(server_addr));
    
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8888);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    //inet_pton(AF_INET,ipAddress,(void*)&server_addr.sin_addr);
    //cout <<"B > 服务器IP: "<<inet_ntoa(server_addr.sin_addr)<<" "<<"监听端口: "<<ntohs(server_addr.sin_port)<<endl;

    if(ret=(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))!=0)
    {
        cout<<"ret: "<<ret<<endl;
        cout<<"bind error..."<<endl;
        close(sockfd);
        exit(ret);
    }

    cout<<"1.bind状态(0为成功): "<<ret<<endl;

    // set the flag=true
    init_flag=true;
}

// void Session_Init::Socket_Init()
// {
//     //create socket
//     if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
//     {
//         perror("socket error...");
//         exit(ret);
//     }

//     //bind socket
//     memset(&server_addr,0,sizeof(server_addr));
    
//     server_addr.sin_family=AF_INET;
//     server_addr.sin_port=htons(8888);
//     server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
//     cout <<"服务器IP: 127.0.0.1"<<" "<<"监听端口: 8888"<<endl;

//     if(ret=bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))!=0)
//     {
//         cout<<"bind error..."<<endl;
//         close(sockfd);
//         exit(ret);
//     }

//     // set the flag=true
//     init_flag=true;
// }

int Session_Init::Get_Sockfd()
{
    return sockfd;
}

struct sockaddr_in Session_Init::Get_Addr()
{
    return server_addr;
}

void Chat_Session::Chat_Online()
{
    MSG msg;
    memset(&msg,0,sizeof(msg));
    int sockfd=Get_Sockfd();
    struct sockaddr_in addr=Get_Addr();
    socklen_t addr_len=sizeof(addr);

    cout <<"02-监听IP: "<<inet_ntoa(addr.sin_addr)<<" "<<"监听端口: "<<ntohs(addr.sin_port)<<endl;
    cout <<"***********************************"<<endl;

    PUSER_LIST head;
    head=Add_Node(&addr);
    cout <<"03-监听IP："<<inet_ntoa(head->addr.sin_addr)<<endl;
    cout <<"++++++++++++++++++++++++++REGON_START++++++++++++++++++++++++++++++"<<endl<<endl;
    while(1)
    {
        static int message_number=1;
        cout<<"4.|||||||||||||||||新的未读消息("<<message_number<<")|||||||||||||||||"<<endl;
        recvfrom(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&addr,&addr_len);
        fprintf(stdout,"5.recvfrom获得的信息类型：%d\n",msg.m_type);
        cout<<"6.客户端来源IP："<<inet_ntoa(addr.sin_addr)<<endl;
        msg.m_name[NAME_LENGTH-1]='\0';
        msg.m_message[TEXT_LENGTH-1]='\0';
        cout<<"7.服务器后台消息print："<<endl;
        fprintf(stdout, "  msg is [%s:%d] => [%d:%s:%s]\n", inet_ntoa(addr.sin_addr),
                          ntohs(addr.sin_port), msg.m_type, msg.m_name, msg.m_message);
        cout<<endl<<endl;
        
        switch(msg.m_type)
        {
            case 1:
            {
                //strcpy(msg.m_message,"login");
                cout<<"8.进入 case 1" <<endl;
                User_Login(head,sockfd,&addr,&msg);
                break;
            }
            case 2:
            {
                cout<<"进入 case 2" <<endl;
                Broadcast_Info(head,sockfd,&addr,&msg);
                break;
            }
            case 3:
            {
                cout<<"进入 case 3" <<endl;
                User_Quit(head,sockfd,&addr,&msg);
                break;
            }
            default:
            {
                fprintf(stdout,"Switch()里的消息类型:",msg.m_type);
                cout <<"发生错误"<<endl;
                fprintf(stderr, "msg is error! [%s:%d] => [%d:%s:%s]\n", inet_ntoa(addr.sin_addr),
                    ntohs(addr.sin_port), msg.m_type, msg.m_name, msg.m_message);
                User_Quit(head,sockfd,&addr,&msg);
                break;
            }
        }
        message_number++;

        //close(sockfd);
        //Destroy_Node(head);
    }
}


// main function
int main(int argc,char **argv)
{
    Session_Init ss;
    //ss.Socket_Init();

    Chat_Session cs;
    cs.Chat_Online();

    return 0;
}