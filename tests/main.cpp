#include <iostream>
#include <string_view>
#include <cstring>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

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
    const char* str="Hello from Server ! \n";
    char s[INET6_ADDRSTRLEN];

    struct addrinfo hints,*servinfo,*p;

    struct sockaddr_storage their_addr;//connectors information
    socklen_t addr_size;//size of their addr?

    int rv;
    memset(&hints,0,sizeof(hints));

    hints.ai_family=AF_INET;//IPv4;
    hints.ai_socktype=SOCK_STREAM;//use stream socket
    hints.ai_flags=AI_PASSIVE;//use this laptops ip

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
        if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1)                                  
        {
            std::cerr<<"Could not Bind to fd\n";
            continue;
        }    
        else
        {
            std::cout<<"Socket Created and binded to fd : "<<sockfd<<"\n";
            break;
        }                                   
    }

    freeaddrinfo(servinfo);
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
            inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s));
            std::cout<<"Server got connection from : "<<s<<"\n";
            if(send(new_fd,str,strlen(str),0)==-1)
            {
                std::cerr<<"Error while sending messege from server !!\n";
                return 1;
            }
            close(new_fd);
        }
    }
    return 0;
}