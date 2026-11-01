#pragma once
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

#include <utility>//std::exchange

class FileDescriptor
{
    private:
        int m_fd=-1;
    public:
        FileDescriptor ()noexcept=default;//empty fd=-1
        explicit FileDescriptor(int fd)noexcept;//takes ownership

        ~FileDescriptor();//close() if possible

        FileDescriptor(const FileDescriptor&)=delete;//no copying cant have 2 sockets 
        FileDescriptor& operator=(const FileDescriptor&)=delete;//no copying again

        FileDescriptor(FileDescriptor&& other)noexcept;//steal set other to -1
        FileDescriptor& operator=(FileDescriptor&& other)noexcept;//close mine first then steal(since move constructor we have to make the object first)

        int get()const noexcept;//borrow the int for send/recv
        int release()noexcept;//give up ownership,return the fd
        void reset(int fd=-1)noexcept;//close current,own the new one
        explicit operator bool()const noexcept;//fd>=0

};
