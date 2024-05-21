#include <echo_handler.hpp>

#include <wheel/log.hpp>

namespace wheel {

bool EchoHandler::process() {
    if (!socket_->recv(buf_)) {
        Log::error("socket recv error");
        return false;
    }

    std::string_view s(buf_);
    if (s == "") {
        Log::info("close({}:{})", socket_->get_peer_ip(), socket_->get_peer_port());
        return false;
    }

    // remove '\n'
    if (s.back() == '\n') {
        s.remove_suffix(1);
    }

    Log::info("recv({}:{}): {}", socket_->get_peer_ip(), socket_->get_peer_port(), s);

    if (!socket_->send(buf_)) {
        Log::error("socket send error");
        return false;
    }
    Log::info("send({}:{}): {}", socket_->get_peer_ip(), socket_->get_peer_port(), s);

    return true;
}

}  // namespace wheel
