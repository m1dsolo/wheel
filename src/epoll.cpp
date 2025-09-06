#include <wheel/epoll.hpp>

namespace wheel {

bool Epoll::add(int fd, uint32_t events) {
    epoll_event ev {
        .events = events,
        .data = {.fd = fd}
    };
    return epoll_ctl(efd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool Epoll::add(int fd, uint32_t events, void* ptr) {
    epoll_event ev {
        .events = events,
        .data = {.ptr = ptr}
    };
    return epoll_ctl(efd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool Epoll::mod(int fd, uint32_t events) {
    epoll_event ev {
        .events = events,
        .data = {.fd = fd}
    };
    return epoll_ctl(efd_, EPOLL_CTL_MOD, fd, &ev) == 0;
}

bool Epoll::mod(int fd, uint32_t events, void *ptr) {
    epoll_event ev {
        .events = events,
        .data = {.ptr = ptr}
    };
    return epoll_ctl(efd_, EPOLL_CTL_MOD, fd, &ev) == 0;
}

bool Epoll::del(int fd) {
    return epoll_ctl(efd_, EPOLL_CTL_DEL, fd, NULL) == 0;
}

int Epoll::wait(int timeout_ms) {
    return epoll_wait(efd_, events_.data(), events_.size(), timeout_ms);
}

int Epoll::get_fd(size_t i) const {
    return events_[i].data.fd;
}

uint32_t Epoll::get_events(size_t i) const {
    return events_[i].events;
}

void* Epoll::get_ptr(size_t i) const {
    return events_[i].data.ptr;
}

}  // namespace wheel
