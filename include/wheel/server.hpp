#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>  // htons
#include <functional>
#include <memory>
#include <unordered_map>

#include <wheel/epoll.hpp>
#include <wheel/log.hpp>
#include <wheel/socket.hpp>
#include <wheel/thread_pool.hpp>

namespace wheel {

class SocketHandler;
class ListenHandler;

class Server {
    friend class ListenHandler;
public:
    Server(std::function<std::shared_ptr<SocketHandler>()> create_handler, int num_threads = 4)
        : create_handler_(create_handler), thread_pool_(num_threads) {}
    ~Server() {}
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    void start(unsigned short port);

private:
    bool init_(unsigned short port);
    void del_socket_(std::shared_ptr<Socket> socket);

    Epoll epoll_;
    bool stop_ = false;
    ThreadPool thread_pool_;
    std::shared_ptr<ListenHandler> listen_handler_;
    std::unordered_map<std::shared_ptr<Socket>, std::shared_ptr<SocketHandler>> handlers_map_;
    std::function<std::shared_ptr<SocketHandler>()> create_handler_;
};

}  // namespace wheel
