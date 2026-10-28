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
    char s[INET6_ADDRSTRLEN];//address of the client IPv6

    struct addrinfo hints,*servinfo,*p;
    /*
     struct addrinfo {
 *      int              ai_flags;      // options: AI_PASSIVE, AI_CANONNAME("listen on every IP of this machine" = NULL node + AI_PASSIVE.)
 *      int              ai_family;     // AF_INET (IPv4), AF_INET6 (IPv6),
 *                                      // AF_UNSPEC (either, hints only)
 *      int              ai_socktype;   // SOCK_STREAM (TCP), SOCK_DGRAM (UDP)
 *      int              ai_protocol;   // 0 in hints = any; results usually
 *                                      // IPPROTO_TCP
 *      socklen_t        ai_addrlen;    // size in bytes of *ai_addr
 *      struct sockaddr *ai_addr;       // the address: IP + port, really a
 *                                      // sockaddr_in or sockaddr_in6
 *      char            *ai_canonname;  // official host name, only with
 *                                      // AI_CANONNAME, else NULL
 *      struct addrinfo *ai_next;       // next node, NULL at end of list
 *  };

 */

    struct sockaddr_storage their_addr;//connectors information i.e the clients address which will be filled in by accept() Big enough for it anywas
    socklen_t addr_size;//socklen_t is an integer type for the size of a socket address, in bytes. On Linux it’s simply a 32-bit unsigned int, defined in <sys/socket.h>.

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

    //NOTE :
    
    /*
    getaddrinfo() uses malloc internally on the result list internally hence freeaddrinfo() must be called to free the memory
    ,This is done since the number of results is unknown in advance
    */

    //NOTE : 

    /*
     Rule: every C API that hands out a resource has a matching release
     function (getaddrinfo/freeaddrinfo, socket/close, fopen/fclose)
    */

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
            /*
 *  inet_ntop()   <arpa/inet.h>   "network to presentation"
 *    Binary IP (bytes inside a socket address) -> text "127.0.0.1"
 *
 *    const char *inet_ntop(int af, const void *src, char *dst,
 *                          socklen_t size);
 *      af    AF_INET / AF_INET6        (their_addr.ss_family)
 *      src   pointer to RAW IP BYTES   (&sin_addr or &sin6_addr,
 *            not the whole struct      -> that's why get_in_addr() exists)
 *      dst   my text buffer            char s[INET6_ADDRSTRLEN] (46)
 *      size  sizeof(dst)
 *      returns dst, or NULL + errno (EAFNOSUPPORT, ENOSPC) -> check it!
 *
 *    No port: use ntohs(((sockaddr_in*)&their_addr)->sin_port)
 *    No DNS: only reformats bytes, never blocks.
 *    Reverse: inet_pton() text -> binary (returns 1 ok, 0 bad text)
 *    Avoid inet_ntoa(): IPv4 only, static buffer, not thread-safe.
 *    All-in-one alternative: getnameinfo(NI_NUMERICHOST|NI_NUMERICSERV).
 */
            std::cout<<"Server got connection from : "<<s<<"\n";
            if(send(new_fd,"Hello From Server <3 ",strlen("Hello From Server <3 "),0)==-1)
            {
                std::cerr<<"Error while sending messege from server !!\n";
                return 1;
            }
            close(new_fd);
        }
    }
    return 0;
}