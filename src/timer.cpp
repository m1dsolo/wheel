#include <wheel/timer.hpp>

#include <chrono>
#include <thread>

namespace wheel {

void Timer::add(time_t interval_us,  timer_func_t func) {
    assert(interval_us >= 0);
    nodes_.emplace(tick() + interval_us, func);
}

time_t Timer::tick() const {
    auto now = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return dur.count();
}

void Timer::pause() {
    if (!pause_) {
        pause_start_tick_ = tick();
        pause_ = true;
    }
}

void Timer::resume() {
    if (pause_) {
        time_t pause_duration = tick() - pause_start_tick_;
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> tmp;
        while (!nodes_.empty()) {
            Node node = std::move(nodes_.top()); nodes_.pop();
            node.expire_time += pause_duration;
            tmp.emplace(node);
        }
        nodes_ = std::move(tmp);
        pause_ = false;
    }
}

std::optional<timer_id_t> Timer::update() {
    if (pause_ || nodes_.empty()) {
        return std::nullopt;
    }

    auto cur = tick();
    if (cur >= nodes_.top().expire_time) {
        Node node = std::move(nodes_.top()); nodes_.pop();
        auto interval = node.func();
        if (interval > 0) {
            node.expire_time = cur + interval;
            nodes_.emplace(node);
        }
        return node.id;
    }
    return std::nullopt;
}

time_t Timer::time_util_next() const {
    if (pause_ || nodes_.empty()) {
        return -1;
    }
    return std::max(static_cast<time_t>(0), nodes_.top().expire_time - tick());
}

void Timer::sleep(time_t us) const {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

}  // namespace wheel
