#include <wheel/server.hpp>

#include <cstring>

#include <wheel/socket_handler.hpp>

namespace wheel {

void Server::start(
    std::function<std::shared_ptr<SocketHandler>()> create_handler,
    unsigned short port,
    int num_threads) {
    if (init_listen_(port)) {
        Log::info("server start on port {}", port);
    } else {
        Log::error("server init listen error");
        return;
    }
    create_handler_ = create_handler;
    thread_pool_.add_thread(num_threads);

    while (!stop_) {
        int n = epoll_.wait();
        for (int i = 0; i < n; ++i) {
            int fd = epoll_.get_fd(i);

            SocketHandler* handler = static_cast<SocketHandler*>(epoll_.get_ptr(i));
            std::shared_ptr<Socket> socket = handler->get_socket();
            uint32_t events = epoll_.get_events(i);
            if (events & EPOLLHUP) {
                Log::info("socket closed by peer");
                del_socket_(socket);
            } else if (events & EPOLLIN) {
                thread_pool_.submit([&]() {
                    if (handler->process()) {
                        epoll_.mod(*handler->get_socket(), EPOLLIN | EPOLLET | EPOLLONESHOT | EPOLLERR, handler);
                    } else {
                        epoll_.mod(*handler->get_socket(), EPOLLHUP, handler);
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

bool Server::init_listen_(unsigned short port) {
    std::shared_ptr<Socket> listen_socket_ = std::make_shared<Socket>();
    if (!listen_socket_->init(SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC)) {
        Log::error("socket init error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    if (!listen_socket_->set_reuse_addr()) {
        Log::error("socket set reuse addr error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    if (!listen_socket_->bind(port)) {
        Log::error("socket bind error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    if (!listen_socket_->listen()) {
        Log::error("socket listen error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    auto listen_handler_ = std::make_shared<ListenHandler>(listen_socket_, this);
    listen_handler_->init(listen_socket_, this);
    handlers_map_[listen_socket_] = listen_handler_;
    if (!epoll_.add(*listen_socket_, EPOLLIN, listen_handler_.get())) {  // maybe EPOLLET
        Log::error("epoll add fd error: {}(errno: {})", std::strerror(errno), errno);
        return false;
    }
    return true;
}

void Server::del_socket_(std::shared_ptr<Socket> socket) {
    if (!epoll_.del(*socket)) {
        Log::error("epoll del fd error: {}(errno: {})", std::strerror(errno), errno);
    }
    handlers_map_.erase(socket);
}

}  // namespace wheel
