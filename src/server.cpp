#include <wheel/server.hpp>

#include <wheel/socket_handler.hpp>

namespace wheel {

void Server::start(unsigned short port) {
    if (!init_(port)) {
        return;
    }
    Log::info("server start on port {}", port);

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
                        // Log::debug("handler process error");
                        epoll_.mod(*handler->get_socket(), EPOLLHUP, handler);
                    }
                });
            } else if (events & EPOLLOUT) {
                // HandlerType* handler = static_cast<HandlerType *>(epoll_.get_ptr(i));
            } else {
                Log::error("unknown event");
                del_socket_(socket);
                continue;
            }
        }
    }
}

bool Server::init_(unsigned short port) {
    std::shared_ptr<Socket> listen_socket_ = std::make_shared<Socket>();
    if (!listen_socket_->init(SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC)) {
        Log::error("socket init error");
        return false;
    }
    if (!listen_socket_->set_reuse_addr()) {
        Log::error("socket set reuse addr error");
        return false;
    }
    if (!listen_socket_->bind(port)) {
        Log::error("socket bind error");
        return false;
    }
    if (!listen_socket_->listen()) {
        Log::error("socket listen error");
        return false;
    }
    listen_handler_ = std::make_shared<ListenHandler>(listen_socket_, this);
    handlers_map_[listen_socket_] = listen_handler_;
    if (!epoll_.add(*listen_socket_, EPOLLIN, listen_handler_.get())) {  // maybe EPOLLET
        Log::error("epoll add fd error");
        return false;
    }
    return true;
}

void Server::del_socket_(std::shared_ptr<Socket> socket) {
    if (!epoll_.del(*socket)) {
        Log::error("epoll del fd error");
    }
    handlers_map_.erase(socket);
}

}  // namespace wheel
