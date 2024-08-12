#include <wheel/timer.hpp>

#include <chrono>
#include <thread>

namespace wheel {

timer_t Timer::tick() const {
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
        timer_t pause_duration = tick() - pause_start_tick_;
        std::priority_queue<TimerNode, std::vector<TimerNode>, std::greater<TimerNode>> tmp;
        while (!nodes_.empty()) {
            TimerNode node = nodes_.top();
            nodes_.pop();
            node.expire_time += pause_duration;
            tmp.push(node);
        }
        nodes_ = std::move(tmp);
        pause_ = false;
    }
}

std::any Timer::update() {
    if (pause_ || nodes_.empty()) {
        return {};
    }

    while (!nodes_.empty() && tick() >= nodes_.top().expire_time) {
        TimerNode node = nodes_.top();
        nodes_.pop();
        std::any res = node.func(node.cnt);
        if (node.cnt == -1 || --node.cnt > 0) {
            node.expire_time = tick() + node.interval;
            nodes_.push(node);
        }
        return res;
    }
    return {};
}

timer_t Timer::time_to_sleep() const {
    if (pause_ || nodes_.empty()) {
        return NO_SLEEP;
    }

    TimerNode node = nodes_.top();
    return node.expire_time >= tick() ? node.expire_time - tick() : 0;
}

void Timer::sleep(timer_t us) const {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

}  // namespace wheel
