#include <iostream>


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

#include <unistd.h>//close()

#define PORT "8080"
#define MAXDATASIZE 100
void* get_in_addr(struct sockaddr* sa)
{
    if(sa->sa_family==AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main()
{
    struct addrinfo hints{},*servinfo,*p;//using {} instead of memset
    int sockfd,numbytes;
    char buf[MAXDATASIZE];
    char address[INET6_ADDRSTRLEN];

    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    if(getaddrinfo("localhost",PORT,&hints,&servinfo)!=0)
    {
        std::cerr<<"Error in getting addresses !\n";
    }
    for(p=servinfo;p!=nullptr;p=p->ai_next)
    {
        if((sockfd=socket(p->ai_family,p->ai_socktype,0))==-1)
        {
            std::cerr<<"Could not create socket \n";
            continue;
        }
        inet_ntop(p->ai_family,get_in_addr((struct sockaddr*)p->ai_addr),address,sizeof(address));
        /*
        The src parameter should be a pointer to either a struct in_addr
         or struct in6_addr containing the address you wish to convert
          to a string.
        */
        std::cout<<"Client attempting connection to "<<address<<"\n";

        if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1)
        {
            std::cerr<<"Could not connect ! \n";
            close(sockfd);
            continue;
        }
        break;
    }
    if(p==nullptr)
    {
        std::cout<<"Client Could not connect to any address ! ";
        close(sockfd);
        return 2;
    }
    inet_ntop(p->ai_family,get_in_addr((struct sockaddr*)p->ai_addr),address,sizeof(address));
    std::cout<<"Client Connected to : "<<address<<"\n";

    freeaddrinfo(servinfo);

    if((numbytes=recv(sockfd,buf,MAXDATASIZE,0))==-1)
    {
        std::cerr<<"Error While Recieving ! \n";
        return 1;
    }
    buf[numbytes] = '\0';
    std::cout<<"Client Recieved : "<<buf;
    close(sockfd);
    return 0;    
}

