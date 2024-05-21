#include <wheel/socket.hpp>

#include <wheel/log.hpp>

namespace wheel {

bool Socket::init(int flags) {
    fd_ = socket(AF_INET, flags, 0);  // TCP
    return fd_ != -1;
}

bool Socket::bind(unsigned short port) {
    return bind("0.0.0.0", port);
}

bool Socket::bind(std::string_view ip, unsigned short port) {
    struct sockaddr_in saddr {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };
    inet_pton(AF_INET, ip.data(), &saddr.sin_addr.s_addr);
    ip_ = ip;
    port_ = port;

    return ::bind(fd_, (struct sockaddr *)&saddr, sizeof saddr) != -1;
}

bool Socket::listen() {
    // 第二个参数为同时能过检测的最大连接数，要小于等于128
    return ::listen(fd_, 128) != -1;
}

std::optional<std::shared_ptr<Socket>> Socket::accept(int flags) {
    struct sockaddr_in caddr;
    socklen_t caddr_len = sizeof caddr;
    int cfd = accept4(fd_, (struct sockaddr *)&caddr, &caddr_len, flags);
    // int cfd = accept4(fd_, (struct sockaddr *)&caddr, &caddr_len, SOCK_CLOEXEC);
    if (cfd == -1)
        return std::nullopt;

    return std::make_shared<Socket>(cfd, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
}

bool Socket::connect(std::string_view ip, unsigned short port) {
    struct sockaddr_in saddr {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };
    inet_pton(AF_INET, ip.data(), &saddr.sin_addr.s_addr);

    return ::connect(fd_, (struct sockaddr *)&saddr, sizeof saddr) != -1;
}

bool Socket::close() {
    if (fd_ == -1)
        return false;
    if (::close(fd_) == -1)
        return false;
    fd_ = -1;
    return true;
}

bool Socket::set_reuse_addr() {
    int reuse = 1;
    return setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) != -1;
}

bool Socket::send(std::string_view s) {
    return ::send(fd_, s.data(), s.length(), 0) != -1;
}

bool Socket::recv(std::span<char> buf) {
    int n = ::recv(fd_, buf.data(), buf.size(), 0);
    if (n == -1) {
        if (errno == EAGAIN) {
            Log::debug("recv: EAGAIN");
            return true;
        } else [[unlikely]] {
            Log::fatal("recv: other");
        }
        return false;
    }
    buf[n] = 0;
    return true;
}

} // namespace wheel
