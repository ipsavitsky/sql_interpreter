#ifndef __SOCK_WRAP_H__
#define __SOCK_WRAP_H__

#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <string>

using socket_t = int;
using port_t = int16_t;

namespace ModelSQL {

class SocketAddress {
   private:
    sockaddr* m_pAddr = nullptr;
    // socklen_t sock_len = 0;
   public:
    explicit SocketAddress(sockaddr*);
    constexpr socklen_t get_length() const;
    sockaddr* get_addr();
};

class BaseSocket {
   public:
    void write(void* buf, size_t len) const;
    void put_char(int c) const;
    void put_string(const char* str) const;
    void put_string(const std::string& s) const;
    int read(void* buf, size_t len) const;
    char get_char() const;
    std::string get_string() const;
    constexpr socket_t get_sock_descriptor() const;
    void set_sock_descriptor(socket_t sock);
   protected:
    socket_t m_Socket = -1;
    SocketAddress* addr = nullptr;
};

class ClientSocket : public BaseSocket {
   public:
    void connect();
};

class ServerSocket : public BaseSocket {
   public:
    BaseSocket accept();
};

class UnClientSocket : public ClientSocket {
   public:
    explicit UnClientSocket(const char* Address);
};

class InClientSocket : public ClientSocket {
   public:
    explicit InClientSocket(const char* HostName, port_t PortNum);
};

class UnServerSocket : public ServerSocket {
   public:
    explicit UnServerSocket(const char* Address, int backlog);
};

class InServerSocket : public ServerSocket {
   public:
    explicit InServerSocket(port_t PortNum, int backlog);
};
};  // namespace ModelSQL
#endif