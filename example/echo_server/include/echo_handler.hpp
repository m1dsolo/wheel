#pragma once

#include <wheel/socket_handler.hpp>

namespace wheel {

class EchoHandler : public SocketHandler {
public:
    EchoHandler(Socket&& socket) : SocketHandler(std::move(socket)) {}
    EchoHandler(const EchoHandler&) = delete;

    virtual bool process() override;

private:
    char buf_[1024];
};

}  // namespace wheel
