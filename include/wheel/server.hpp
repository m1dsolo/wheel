#pragma once

#include <wheel/epoll.hpp>
#include <wheel/log.hpp>
#include <wheel/socket.hpp>
#include <wheel/thread_pool.hpp>
#include <wheel/socket_handler.hpp>

#include <sys/socket.h>
#include <memory>
#include <unordered_map>
#include <cstring>

namespace wheel {

template <typename HandlerType> requires std::is_base_of_v<SocketHandler, HandlerType>
class Server;

template <typename HandlerType> requires std::is_base_of_v<SocketHandler, HandlerType>
class ListenHandler : public SocketHandler {
public:
    ListenHandler(Socket&& socket, Server<HandlerType>& server) : SocketHandler(std::move(socket)), server_(server) {}
    ~ListenHandler() = default;

    virtual bool process() override; 

private:
    Server<HandlerType>& server_;
};

template <typename HandlerType> requires std::is_base_of_v<SocketHandler, HandlerType>
class Server {
    friend class ListenHandler<HandlerType>;

public:
    Server() = default;
    ~Server() = default;

    void start(unsigned short port, int num_threads);

private:
    bool init_listen_(unsigned short port);
    void del_socket_(Socket& socket);

    Epoll epoll_;
    bool stop_ = false;
    ThreadPool thread_pool_;
    std::unordered_map<int, std::unique_ptr<SocketHandler>> handlers_map_;  // {fd, handler}
};

template <typename HandlerType> requires std::is_base_of_v<SocketHandler, HandlerType>
void Server<HandlerType>::start(unsigned short port, int num_threads) {
    if (init_listen_(port)) {
        Log::info("server start on port {}", port);
    } else {
        Log::error("server init listen error");
        return;
    }
    thread_pool_.add_thread(num_threads);

    while (!stop_) {
        int n = epoll_.wait();
        for (int i = 0; i < n; ++i) {
            int fd = epoll_.get_fd(i);

            SocketHandler* handler = static_cast<SocketHandler*>(epoll_.get_ptr(i));
            auto& socket = handler->socket();
            uint32_t events = epoll_.get_events(i);
            if (events & EPOLLHUP) {
                Log::info("socket closed by peer");
                del_socket_(socket);
            } else if (events & EPOLLIN) {
                thread_pool_.submit([this, handler]() {
                    if (handler->process()) {
                        epoll_.mod(handler->socket(), EPOLLIN | EPOLLET | EPOLLONESHOT | EPOLLERR, handler);
                    } else {
                        epoll_.mod(handler->socket(), EPOLLHUP, handler);
                    }
                });
            } else if (events & EPOLLOUT) {

            } else {
                Log::error("unknown event");
                del_socket_(socket);
            }
        }
    }
}

template <typename HandlerType> requires std::is_base_of_v<SocketHandler, HandlerType>
bool Server<HandlerType>::init_listen_(unsigned short port) {
    Socket listen_socket_;
    if (!listen_socket_.init(SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC)) {
        Log::error("socket init error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    if (!listen_socket_.set_reuse_addr()) {
        Log::error("socket set reuse addr error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    if (!listen_socket_.bind("0.0.0.0", port)) {
        Log::error("socket bind error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    if (!listen_socket_.listen()) {
        Log::error("socket listen error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    int fd = listen_socket_.fd();
    auto listen_handler_ = std::make_unique<ListenHandler<HandlerType>>(std::move(listen_socket_), *this);
    if (!epoll_.add(fd, EPOLLIN, listen_handler_.get())) {  // maybe EPOLLET
        Log::error("epoll add fd error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    handlers_map_[fd] = std::move(listen_handler_);
    return true;
}

template <typename HandlerType> requires std::is_base_of_v<SocketHandler, HandlerType>
void Server<HandlerType>::del_socket_(Socket& socket) {
    if (!epoll_.del(socket)) {
        Log::error("epoll del fd error: {}(errno: {})", std::strerror(errno), errno);
    }
    handlers_map_.erase(socket);
}

template <typename HandlerType> requires std::is_base_of_v<SocketHandler, HandlerType>
bool ListenHandler<HandlerType>::process() {
    if (auto ret = socket_.accept(SOCK_NONBLOCK | SOCK_CLOEXEC)) {
        auto socket = std::move(*ret);
        Log::info("new connection from {}:{}", socket.get_peer_ip(), socket.get_peer_port());
        auto fd = socket.fd();

        auto handler = std::make_unique<HandlerType>();
        handler->set_socket(std::move(socket));
        if (!server_.epoll_.add(fd, EPOLLIN | EPOLLET | EPOLLONESHOT, handler.get())) {
            Log::error("epoll add fd error");
            return false;
        }
        server_.handlers_map_[fd] = std::move(handler);
        return true;
    } else {
        if (errno == EAGAIN) {
            return true;
        }
        Log::error("socket accept error");
        return false;
    }
}

}  // namespace wheel
