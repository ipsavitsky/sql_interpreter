#include "sockets.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

#include "../exceptions/exceptions.hpp"

using namespace ModelSQL;

SocketAddress::SocketAddress(sockaddr* in) : m_pAddr{in} {}

constexpr socklen_t SocketAddress::get_length() const {
    return sizeof(*m_pAddr);
}

sockaddr* SocketAddress::get_addr() { return m_pAddr; }

void BaseSocket::write(void* buf, size_t len) const {
    if (send(m_Socket, buf, len, 0) == -1) {
        throw serv_except::send_error(errno);
    }
}

void BaseSocket::put_char(int c) const {
    if (send(m_Socket, &c, sizeof(int), 0) == -1) {
        throw serv_except::send_error(errno);
    }
}

void BaseSocket::put_string(const char* str) const {
    if (send(m_Socket, str, strlen(str), 0) == -1) {
        throw serv_except::send_error(errno);
    }
}

void BaseSocket::put_string(const std::string& s) const {
    if (send(m_Socket, s.c_str(), s.size(), 0) == -1) {
        throw serv_except::send_error(errno);
    }
}

int BaseSocket::read(void* buf, size_t len) const {
    int res;
    if ((res = recv(m_Socket, buf, len, 0)) == -1) {
        throw serv_except::recv_error(errno);
    }
    return res;
}

char BaseSocket::get_char() const {
    char res;
    if (recv(m_Socket, &res, sizeof(res), 0) == -1) {
        throw serv_except::recv_error(errno);
    }
    return res;
}

std::string BaseSocket::get_string() const {
    int count = 1024;
    std::string str_res;
    str_res.resize(count);
    std::cerr << "detected " << count << " bytes" << std::endl;
    if (::recv(m_Socket, str_res.data(), count, 0) == -1) {
        throw serv_except::recv_error(errno);
    }
    str_res.shrink_to_fit();
    return str_res;
}

constexpr socket_t BaseSocket::get_sock_descriptor() const { return m_Socket; }

void BaseSocket::set_sock_descriptor(socket_t sock) { m_Socket = sock; }

void ClientSocket::connect() {
    ::connect(m_Socket, addr->get_addr(), addr->get_length());
}

BaseSocket ServerSocket::accept() {
    BaseSocket retval;
    sockaddr address;
    socklen_t len = sizeof(address);
    auto new_sock = ::accept(m_Socket, &address, &len);
    if (new_sock == -1) {
        throw serv_except::accept_error(errno);
    }
    retval.set_sock_descriptor(new_sock);
    return retval;
}

UnClientSocket::UnClientSocket(const char* Address) {
    if ((m_Socket = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        throw serv_except::socket_error(errno);
    }
    sockaddr_un name = {.sun_family = AF_UNIX};
    strcpy(name.sun_path, Address);
    addr = new SocketAddress(reinterpret_cast<sockaddr*>(&name)); //NOLINT
}

InClientSocket::InClientSocket(const char* HostName, port_t PortNum) {
    if ((m_Socket = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        throw serv_except::socket_error(errno);
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PortNum);
    if (inet_pton(AF_INET, HostName, &address.sin_addr) <= 0) {
        throw serv_except::convert_error(errno);
    }
    addr = new SocketAddress(reinterpret_cast<sockaddr*>(&address)); //NOLINT
}

UnServerSocket::UnServerSocket(const char* Address, int backlog) {
    std::cerr << "UnServerSocket()" << std::endl;
    std::cerr << "socket()" << std::endl;
    if ((m_Socket = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        throw serv_except::socket_error(errno);
    }
    sockaddr_un name = {.sun_family = AF_UNIX};
    strcpy(name.sun_path, Address);
    unlink(Address);
    addr = new SocketAddress(reinterpret_cast<sockaddr*>(&name));  // NOLINT
    std::cerr << "address allocated" << std::endl;
    if (::bind(m_Socket, addr->get_addr(), addr->get_length()) < 0) {
        throw serv_except::bind_error(errno);
    }
    std::cerr << "listen()" << std::endl;
    if (::listen(m_Socket, backlog) < 0) {
        throw serv_except::listen_error(errno);
    }
}

InServerSocket::InServerSocket(port_t PortNum, int backlog) {
    if ((m_Socket = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        throw serv_except::socket_error(errno);
    }
    sockaddr_in address = {.sin_family = AF_INET,
                           .sin_port = htons(PortNum),
                           .sin_addr = {.s_addr = INADDR_ANY}};
    addr = new SocketAddress(reinterpret_cast<sockaddr*>(&address));  // NOLINT
    if (::bind(m_Socket, addr->get_addr(), addr->get_length()) < 0) {
        throw serv_except::bind_error(errno);
    }
    if (::listen(m_Socket, backlog) < 0) {
        throw serv_except::listen_error(errno);
    }
}
