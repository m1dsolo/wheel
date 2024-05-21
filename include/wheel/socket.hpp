#pragma once

#include <arpa/inet.h>  // htons
#include <sys/socket.h>
#include <unistd.h>  // close
#include <memory>
#include <optional>
#include <span>  // c++20
#include <string>

#include <wheel/epoll.hpp>

namespace wheel {

class Socket {
public:
    Socket() {}
    explicit Socket(int fd) : fd_(fd) {}
    ~Socket() { close(); }
    Socket(int fd, std::string_view ip, unsigned short port) : fd_(fd), ip_(ip), port_(port) {}

    operator int() const { return fd_; }

    bool init(int flags = 0);
    bool bind(unsigned short port);
    bool bind(std::string_view ip, unsigned short port);
    bool listen();
    std::optional<std::shared_ptr<Socket>> accept(int flags = 0);
    bool connect(std::string_view ip, unsigned short port);
    bool close();

    bool set_reuse_addr();

    bool send(std::string_view s);
    bool recv(std::span<char> buf);

    const std::string& get_peer_ip() const { return ip_; }
    unsigned short get_peer_port() const { return port_; }

    // std::string_view get_buf() const { return buf_; }
    // void set_buf(std::string_view s) { s.copy(buf_, s.size()); }

private:
    int fd_ = -1;
    std::string ip_ = "";
    unsigned short port_ = 0;
};

}  // namespace wheel
