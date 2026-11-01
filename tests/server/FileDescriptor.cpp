#include "FileDescriptor.h"

FileDescriptor::FileDescriptor(int fd)noexcept
    :m_fd{fd}
{
    std::cout<<"File Descriptor Created.\n";
}
void FileDescriptor::reset(int fd)noexcept
{
    if(m_fd>=0&&m_fd!=fd)
    {
        ::close(m_fd);//means global close from <unistd.h>
    }
    m_fd=fd;
}
FileDescriptor::~FileDescriptor()
{
    reset();
}

 FileDescriptor::FileDescriptor(FileDescriptor&& other)noexcept
    :m_fd{std::exchange(other.m_fd,-1)}
 {

 }

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other)noexcept
{
    reset(other.release());
    return *this;
}

int FileDescriptor::get()const noexcept
{
    return m_fd;
}
int  FileDescriptor::release() noexcept   { return std::exchange(m_fd, -1); }
FileDescriptor::operator bool()const noexcept{return m_fd>=0;}

