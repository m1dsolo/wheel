#pragma once

#include <wheel/socket_handler.hpp>
#include <wheel/ring_buffer.hpp>

namespace wheel {

class MessageHandler : public SocketHandler {
public:
    MessageHandler() = default;
    virtual ~MessageHandler() = default;

    bool process() override;

    virtual bool process(std::string_view msg) = 0;
    static bool send(wheel::Socket& socket, std::string_view msg);

protected:
    RingBuffer<char, 4096> ring_buffer_;
    char buf_[1024];

    bool recv_();
};

}  // namespace wheel
