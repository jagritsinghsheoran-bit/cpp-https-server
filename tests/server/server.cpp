#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>   // basic system types: ssize_t, pid_t, size_t.
                         // Old Unix required it before socket headers;
                         // POSIX no longer does. Harmless, often dropped.]

#include <sys/socket.h>  // core socket API: socket, bind, listen, accept,
                         // connect, send, recv, setsockopt; sockaddr,
                         // sockaddr_storage, socklen_t; AF_*, SOCK_*,
                         // SOL_SOCKET, SO_REUSEADDR

#include <netinet/in.h>  // internet address structs: sockaddr_in,
                         // sockaddr_in6, in_addr, in6_addr; INADDR_ANY,
                         // IPPROTO_TCP, INET_ADDRSTRLEN / INET6_ADDRSTRLEN

#include <netdb.h>       // name and service lookup: getaddrinfo,
                         // freeaddrinfo, gai_strerror, getnameinfo;
                         // struct addrinfo, AI_PASSIVE, AI_CANONNAME

#include <arpa/inet.h>   // address conversion: inet_ntop, inet_pton;
                         // byte order: htons, htonl, ntohs, ntohl



namespace constants
{
    constexpr std::size_t maxBufferSize=1024;
}
int send_all(std::size_t fd,const char* buffer,std::size_t buffer_len,[[maybe_unused]]int flag)
{
    std::size_t len{0};
    while(len<buffer_len)
    {
        len+=send(fd,buffer,buffer_len-len,0);
        std::cout<<"Completed : "<<len<<" bytes \n";
    }
    
    if(len>=buffer_len)
    {
        std::cout<<"Send completed : "<<buffer_len<<" bytes.\n";
        return 0;
    }
    else
    {
        std::cout<<"Could not send err\n";
        return -1;
    }
}


//beejs get_in_addr
void *get_in_addr(struct sockaddr *sa)
{
    if(sa->sa_family==AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main()
{
    int sockfd;//listening socket
    int new_fd;//new Connection socket
    const int reuse=1;//for setsockopt

    char buf[constants::maxBufferSize];//Clients incoming data 

    char s[INET6_ADDRSTRLEN];//address of the client IPv6

    struct addrinfo hints,*servinfo,*p;
   
    struct sockaddr_storage their_addr;
    socklen_t addr_size;

    int rv;
    memset(&hints,0,sizeof(hints));//Fill the struct with empty values

    hints.ai_family=AF_UNSPEC;//IPv4;
    hints.ai_socktype=SOCK_STREAM;//use stream socket
    hints.ai_flags=AI_PASSIVE;//use this laptops ip or wildcard 0.0.0.0/8080

    if((rv=getaddrinfo(NULL,"8080",&hints,&servinfo))!=0)//use port 8080
    {
        std::cerr<<"Error during DNS lookup\n";
        return 1;
    }

    for(p=servinfo;p!=nullptr;p=p->ai_next)
    {
        if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1)
        {
            std::cerr<<"Could not create Socket\n";
            continue;
        }
        if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0)
        {
            std::cerr<<"setsockopt(SO_REUSEADDR) Failed\n";
            close(sockfd);
            continue;
        }
        if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1)                                  
        {
            std::cerr<<"Could not Bind to fd\n";
            close(sockfd);
            continue;
        }    
        else
        {
            std::cout<<"Socket Created and binded to fd : "<<sockfd<<"\n";
            break;
        }                                   
    }

    
    freeaddrinfo(servinfo);//free the linked list
    if(p==nullptr)
    {
        std::cout<<"Server failed to bind\n";
        return 1;
    }
    if(listen(sockfd,5)==-1)
    {
        std::cout<<"Could not Listen\n";
        return 1;
    }
    while(1)
    {
        addr_size=sizeof(their_addr);
        new_fd=accept(sockfd,(struct sockaddr *)&their_addr,&addr_size);
        if(new_fd==-1)
        {
            std::cout<<"Error while accepting\n";
            continue;
        }
        else
        {
            inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s));
            std::cout<<"Server got connection from : "<<s<<"\n";
            while(1)
            {
                std::size_t n=recv(new_fd,buf,constants::maxBufferSize,0);
                if(n>0)
                {
                    if(send_all(new_fd,buf,strlen(buf),0)==-1)
                    {
                        std::cerr<<"Error while sending messege from server !!\n";

                    }   
                }
                else if(n==0)
                {
                    break;
                }
                else
                {
                    std::cerr<<"Error While recieveing !\n";
                    break;
                }
            }
             std::cout<<"Client Closed Connection ! \n";
             close(new_fd);
        }
    }
    return 0;
}