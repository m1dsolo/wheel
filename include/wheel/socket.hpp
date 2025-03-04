#pragma once

#include <wheel/epoll.hpp>

#include <arpa/inet.h>  // htons
#include <sys/socket.h>
#include <unistd.h>  // close
#include <optional>
#include <span>  // c++20
#include <string>

namespace wheel {

class Socket {
public:
    Socket() {}
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) : fd_(other.fd_), ip_(std::move(other.ip_)), port_(other.port_) {
        other.fd_ = -1;
        other.port_ = 0;
    }
    Socket& operator=(Socket&& other) {
        if (this != &other) {
            fd_ = other.fd_;
            ip_ = std::move(other.ip_);
            port_ = other.port_;
            other.fd_ = -1;
            other.port_ = 0;
        }
        return *this;
    }
    ~Socket() { close(); }

    operator int() const { return fd_; }

    bool init(int flags = 0);
    bool bind(std::string_view ip, unsigned short port);
    bool listen();
    std::optional<Socket> accept(int flags = 0);
    bool connect(std::string_view ip, unsigned short port);
    bool close();

    bool set_reuse_addr();

    int send(std::string_view s);
    int recv(std::span<char> buf);

    const std::string& get_peer_ip() const { return ip_; }
    unsigned short get_peer_port() const { return port_; }

    // std::string_view get_buf() const { return buf_; }
    // void set_buf(std::string_view s) { s.copy(buf_, s.size()); }

    int fd() const { return fd_; }

private:
    Socket(int fd, std::string_view ip, unsigned short port) : fd_(fd), ip_(ip), port_(port) {}

    int fd_ = -1;
    std::string ip_ = "";
    unsigned short port_ = 0;
};

}  // namespace wheel
