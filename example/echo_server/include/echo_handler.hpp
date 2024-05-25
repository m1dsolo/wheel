#pragma once

#include <wheel/socket_handler.hpp>

namespace wheel {

class EchoHandler : public SocketHandler {
public:
    EchoHandler() = default;
    ~EchoHandler() = default;
    EchoHandler(const EchoHandler&) = delete;
    EchoHandler& operator=(const EchoHandler&) = delete;

    virtual bool process() override;

private:
    char buf_[1024];
};

}  // namespace wheel
