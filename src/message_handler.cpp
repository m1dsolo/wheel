#include <wheel/message_handler.hpp>
#include <wheel/log.hpp>

namespace wheel {

bool MessageHandler::process() {
    if (recv_()) {
        if (ring_buffer_.size() >= 4) {
            ring_buffer_.peek(buf_, 4);
            int size = std::stoi(std::string(buf_, 4));
            if (ring_buffer_.size() >= size + 4) {
                ring_buffer_.pop(4);
                ring_buffer_.get(buf_, size);
                if (!process(std::string_view(buf_, size))) {
                    return false;
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

bool MessageHandler::send(wheel::Socket& socket, std::string_view msg) {
    // TODO: optimize
    std::string s = std::to_string(msg.size());
    s.insert(s.begin(), 4 - s.size(), '0');
    s += msg;
    wheel::Log::info("send({}:{}): {}", socket.get_peer_ip(), socket.get_peer_port(), msg);
    if (socket.send(s) == -1) {
        wheel::Log::error("socket send error");
        return false;
    }
    return true;
}

bool MessageHandler::recv_() {
    int n = socket_.recv(buf_);
    if (n > 0) {
        ring_buffer_.put(buf_, n);
        buf_[n] = 0;
        wheel::Log::info("recv({}:{}): {}", socket_.get_peer_ip(), socket_.get_peer_port(), buf_);
    } else if (n == 0) {
        wheel::Log::info("connection closed by peer");
        return false;
    } else {
        wheel::Log::error("socket recv error");
        return false;
    }
    return true;
}

}  // namespace wheel
