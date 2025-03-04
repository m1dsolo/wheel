#pragma once

#include <wheel/epoll.hpp>
#include <wheel/socket.hpp>

namespace wheel {

class SocketHandler {
public:
    SocketHandler() = default;
    SocketHandler(Socket&& socket) : socket_(std::move(socket)) {}
    virtual ~SocketHandler() = default;

    virtual bool process() = 0;

    void set_socket(Socket&& socket) { socket_ = std::move(socket); }
    Socket& socket() { return socket_; }

protected:
    Socket socket_;
};

}  // namespace wheel
