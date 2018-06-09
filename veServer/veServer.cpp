#include "veServer.h"

PUSER_LIST Chat_Session::Add_Node(struct sockaddr_in *paddr)
{
    //cout<<"Add_Node_ADDR: "<<inet_ntoa(paddr->sin_addr)<<endl;
    PUSER_LIST node=(PUSER_LIST)malloc(sizeof(USER_LIST));    
    if(NULL == node)
    {
        cout<<"Chat_Session::Add_Node.malloc error..."<<endl;
    }
    node->addr = *paddr;
    return node;
}

void Chat_Session::Insert_Node(PUSER_LIST phead,struct sockaddr_in *paddr)
{
    //cout<<"InsertLogin_IP_ADDR: "<<inet_ntoa(paddr->sin_addr)<<endl;
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
    phead=NULL;
}

void Chat_Session::User_Login(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg)
{
    cout <<"*** Function Info: Chat_Session::User_Login"<<endl;
    Insert_Node(phead, paddr);
    cout <<"*** Node Info: "<<endl;
    cout << "    { "<<endl;
    cout <<"      Insert_Node,phead address: "<<phead<<endl;
    cout <<"      Insert_Node,phead->addr.sin_addr: "<<inet_ntoa(phead->addr.sin_addr)<<endl;
    cout <<"      Insert_Node,phead->next info: "<<phead->next<<endl;
    //PUSER_LIST ptr=phead;
    phead = phead->next;
    cout <<"      phead = phead->next,phead address: "<<phead<<endl;
    cout <<"      phead = phead->next,phead->addr.sin_addr: "<<inet_ntoa(phead->addr.sin_addr)<<endl;
    cout <<"      phead = phead->next,phead->next info: "<<phead->next<<endl;
    cout << "    } "<<endl;
    // cout <<endl;
    // cout <<"@@@@@@@@@@@@@@@@@@@@@ Message End @@@@@@@@@@@@@@@@@@@@@"<<endl<<endl;
    //每次新增加结点之后,phead总是指向尾结点，新增的结点
    //Since then only for the previous linked list
    while(phead->next)
    {
        phead = phead->next;
        //ptr=ptr->next;
        sendto(sockfd, msg, sizeof(*msg), 0, (struct sockaddr*)&phead->addr, sizeof(struct sockaddr_in));
    }
}

void Chat_Session::Broadcast_Info(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg)
{
    cout <<"*** Function Info: Chat_Session::Broadcast_Info"<<endl;
    int flag = 0; //1 type found
    //cout<<endl;
    //cout<<"paddr传入地址外层测试: "<<inet_ntoa(paddr->sin_addr)<<endl;
    while(phead->next)
    {
        phead = phead->next;
        //cout<<"phead广播地址测试: "<<inet_ntoa(phead->addr.sin_addr)<<endl;
        //cout<<"paddr传入地址内层测试: "<<inet_ntoa(paddr->sin_addr)<<endl;
        if((flag) || !(flag=memcmp(paddr, &phead->addr, sizeof(struct sockaddr_in))==0))
        {
            //cout<<inet_ntoa(phead->addr.sin_addr)<<endl;
            sendto(sockfd, msg, sizeof(*msg), 0, (struct sockaddr*)&phead->addr, sizeof(struct sockaddr_in));
        }
    }
    //cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl<<endl;
}

void Chat_Session::User_Quit(PUSER_LIST phead,int sockfd,struct sockaddr_in *paddr,PMSG msg)
{
    cout <<"*** Function Info: Chat_Session::User_Quit"<<endl;
    int flag = 0;//1 type found
    //cout <<endl;
    while(phead->next)
    {
        if((flag) || !(flag = memcmp(paddr, &phead->next->addr, sizeof(struct sockaddr_in))==0))
        {
            sendto(sockfd, msg, sizeof(*msg), 0, (struct sockaddr*)&phead->next->addr, sizeof(struct sockaddr_in));
            phead = phead->next;
        }        
        else
        { //delete user(quit)
            PUSER_LIST tmp_node = phead->next;
            phead->next = tmp_node->next;
            free(tmp_node);
        }
    }
    //cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl<<endl;        
}

Session_Init::Session_Init()
{
    //create socket
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("socket error...\n");
        cout <<"socket ret: "<<sockfd<<endl;
        exit(ret);
    }

    //char *ipAddress="127.0.0.1";
    int on = 1; 
    int opt_ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    if(opt_ret!=0)
    {
        perror("setsockopt error...\n");
        cout <<"bind ret: "<<opt_ret<<endl;
        close(sockfd);
        exit(ret);
    }

    //bind socket
    memset(&server_addr,0,sizeof(server_addr));

    // atoi()
    int listen_port=atoi(G_Port);
    
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(listen_port);

    if(strcmp(G_IP_Addr,"INADDR_ANY")==0)
    {
        server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    }
    else
    {
        inet_pton(AF_INET,G_IP_Addr,(void*)&server_addr.sin_addr);
    }
    //inet_pton(AF_INET,ipAddress,(void*)&server_addr.sin_addr);
    //cout <<"B > 服务器IP: "<<inet_ntoa(server_addr.sin_addr)<<" "<<"监听端口: "<<ntohs(server_addr.sin_port)<<endl;

    int bind_ret;
    if(bind_ret=(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))!=0)
    {
        perror("bind error...\n");
        cout <<"bind ret: "<<bind_ret<<endl;
        close(sockfd);
        exit(ret);
    }

    //cout<<"1.bind状态(0为成功): "<<ret<<endl;
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
    return server_addr;
}

Chat_Session::Chat_Session()
{
    ret=-1;
}

Chat_Session::~Chat_Session()
{

}

void Chat_Session::Chat_Online()
{
    MSG msg;
    memset(&msg,0,sizeof(msg));
    int sockfd=Get_Sockfd();
    struct sockaddr_in addr=Get_Addr();
    socklen_t addr_len=sizeof(addr);

    cout <<endl;
    cout <<"[监听IP]: "<<inet_ntoa(addr.sin_addr);
    cout <<"                    ";
    cout <<"[监听端口]: "<<ntohs(addr.sin_port)<<endl<<endl;

    if(init_flag)
    {
        cout <<"[连接状态]: \033[32m"<<CONNECCT_STATUS_NORMAL<<"\033[0m"<<endl;
    }
    else
    {
        cout <<"[连接状态]: \033[31m"<<CONNECCT_STATUS_ERROR<<"\033[0m"<<endl;
    }
    cout <<endl;

    PUSER_LIST head;
    head=Add_Node(&addr);
    //cout <<"03-监听IP："<<inet_ntoa(head->addr.sin_addr)<<endl;
    cout <<"++++++++++++++++++++++++++ Message Info ++++++++++++++++++++++++++++++"<<endl<<endl;
    while(1)
    {
        static int message_number=1;
        cout<<"||||||||||||||||| New Message ("<<message_number<<") |||||||||||||||||"<<endl<<endl;
        recvfrom(sockfd,&msg,sizeof(msg),0,(struct sockaddr*)&addr,&addr_len);
        fprintf(stdout,"*** Recvfrom Message Type: %d\n",msg.m_type);
        cout<<"*** Client Source IP: "<<inet_ntoa(addr.sin_addr)<<":"<<ntohs(addr.sin_port)<<endl;
        msg.m_name[NAME_LENGTH-1]='\0';
        msg.m_message[MESSAGE_LENGTH-1]='\0';
        cout<<"*** Message Info: "<<endl;
        //cout<<"    "<<"[ "<<msg.m_type<<":"<<msg.m_name<<":"<<msg.m_message<<" ]";
        cout << "    { "<<endl;
        cout << "      Type: "<<msg.m_type<<endl;
        cout << "      Name: "<<msg.m_name<<endl;
        //cout << "      Message: "<<msg.m_message;
        if(1==msg.m_type)
        {
            cout << "      Message: "<<"\033[32m"<<msg.m_message<<"\033[0m";
        }
        else if(2==msg.m_type)
        {
            cout << "      Message: "<<"\033[33m"<<msg.m_message<<"\033[0m";
        }
        else if(3==msg.m_type)
        {
            cout << "      Message: "<<"\033[31m"<<msg.m_message<<"\033[0m";
        }
        cout << "    } "<<endl;
        //cout<<endl<<endl;
        
        //PUSER_LIST head_node=head;
        switch(msg.m_type)
        {
            case 1:
            {
                //cout<<"8.进入 case 1" <<endl;
                //cout<<head->addr<<endl;
                //cout<<head->next<<endl;
                User_Login(head,sockfd,&addr,&msg);
                cout <<endl;
                cout <<"################# Message ("<<message_number<<")" <<" End ################# "<<endl<<endl;
                cout <<endl;
                break;
            }
            case 2:
            {
                //cout<<"进入 case 2" <<endl;
                Broadcast_Info(head,sockfd,&addr,&msg);
                cout <<endl;
                cout <<"################# Message ("<<message_number<<")" <<" End ################# "<<endl<<endl;
                cout <<endl;
                break;
            }
            case 3:
            {
                //cout<<"进入 case 3" <<endl;
                User_Quit(head,sockfd,&addr,&msg);
                cout <<endl;
                cout <<"################# Message ("<<message_number<<")" <<" End ################# "<<endl<<endl;
                cout <<endl;
                break;
            }
            default:
            {
                cout <<"*** Error Info: "<<endl;
                cout << "    { "<<endl;
                cout << "      Addr: "<<inet_ntoa(addr.sin_addr)<<":"<<ntohs(addr.sin_port)<<endl;
                cout << "      Type: "<<msg.m_type<<endl;
                cout << "      Name: "<<msg.m_name<<endl;
                cout << "      Message: "<<msg.m_message;
                cout << "    } "<<endl;
                fprintf(stderr, "*** Msg_Info Error:\n    {\n      Addr: %s:%d\n      Type: %d\n      Name: %s\n      Message: %s    }", inet_ntoa(addr.sin_addr),
                     ntohs(addr.sin_port), msg.m_type, msg.m_name, msg.m_message);
                User_Quit(head,sockfd,&addr,&msg);
                cout <<endl;
                cout <<"################# Message ("<<message_number<<")" <<" End ################# "<<endl<<endl;
                cout <<endl;
                break;
            }
        }
        message_number++;
    }
    close(sockfd);
    Destroy_Node(head);
}

// main function
int main(int argc,char **argv)
{
    // default value: ip:INADDR_ANY port:8888
    switch(argc)
    {
        case 1:
        {
            strncpy(G_IP_Addr,"INADDR_ANY",IP_LENGTH-1);
            strncpy(G_Port,"8888",PORT_LENGTH-1);
            break;
        }
        case 2:
        {
            strncpy(G_IP_Addr,argv[1],IP_LENGTH-1);
            strncpy(G_Port,"8888",PORT_LENGTH-1);
            break;
        }
        case 3:
        {
            strncpy(G_IP_Addr,argv[1],IP_LENGTH-1);
            strncpy(G_Port,argv[2],PORT_LENGTH-1);
            break;
        }
    }

    Session_Init ss;

    Chat_Session cs;
    cs.Chat_Online();

    return 0;
}