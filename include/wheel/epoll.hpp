#pragma once

#include <sys/epoll.h>
#include <unistd.h>  // close
#include <vector>

namespace wheel {

class Epoll {
public:
    explicit Epoll(int maxEvents = 10)
        : efd_(epoll_create1(EPOLL_CLOEXEC)), events_(maxEvents) {}
    ~Epoll() { close(efd_); }

    bool add(int fd, uint32_t events);
    bool add(int fd, uint32_t events, void* ptr);
    bool mod(int fd, uint32_t events);
    bool mod(int fd, uint32_t events, void *ptr);
    bool del(int fd);

    int wait(int timeout_ms = -1);
    int get_fd(size_t i) const;
    uint32_t get_events(size_t i) const;
    void* get_ptr(size_t i) const;

private:
    int efd_;
    std::vector<epoll_event> events_;
};

}  // namespace wheel
