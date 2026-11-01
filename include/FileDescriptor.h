#pragma once

#include <unistd.h>


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
