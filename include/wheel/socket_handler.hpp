#pragma once

#include <wheel/epoll.hpp>
#include <wheel/socket.hpp>

namespace wheel {

class Server;

class SocketHandler {
public:
    SocketHandler() {}
    SocketHandler(std::shared_ptr<Socket> socket, Server* server) : socket_(socket), server_(server) {}
    virtual ~SocketHandler() {}
    SocketHandler(const SocketHandler &) = delete;
    SocketHandler &operator=(const SocketHandler &) = delete;

    virtual bool process() = 0;

    void init(std::shared_ptr<Socket> socket, Server* server) { socket_ = socket; server_ = server; }
    std::shared_ptr<Socket>& get_socket() { return socket_; }

protected:
    std::shared_ptr<Socket> socket_;
    Server* server_ = nullptr;
};

class ListenHandler : public SocketHandler {
public:
    ListenHandler() {}
    ListenHandler(std::shared_ptr<Socket> socket, Server* server) : SocketHandler(socket, server) {}
    ~ListenHandler() {}
    ListenHandler(const ListenHandler &) = delete;
    ListenHandler &operator=(const ListenHandler &) = delete;

    virtual bool process() override; 
};

}  // namespace wheel
