#include <wheel/socket_handler.hpp>

#include <wheel/log.hpp>
#include <wheel/server.hpp>

namespace wheel {

bool ListenHandler::process() {
    auto ret = socket_->accept(SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (!ret) {
        if (errno == EAGAIN) {
            return true;
        }
        Log::error("socket accept error");
        return false;
    }
    auto socket = *ret;
    std::shared_ptr<SocketHandler> handler = server_->create_handler_();
    handler->init(socket, server_);

    server_->handlers_map_[socket] = handler;
    if (!server_->epoll_.add(*socket, EPOLLIN | EPOLLET | EPOLLONESHOT, handler.get())) {
        Log::error("epoll add fd error");
        return false;
    }

    Log::info("new connection from {}:{}", socket->get_peer_ip(), socket->get_peer_port());

    return true;
}

}  // namespace wheel
