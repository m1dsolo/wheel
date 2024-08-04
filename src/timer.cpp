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
        timer_t pause_tick_ = tick() - pause_start_tick_;
        std::set<TimerNode> tmp;
        std::transform(nodes_.begin(), nodes_.end(), std::inserter(tmp, tmp.begin()), [pause_tick_](const TimerNode& node) {
            return TimerNode(node.expire_time + pause_tick_, node.interval, node.cnt, node.func);
        });
        nodes_ = std::move(tmp);

        pause_ = false;
    }
}

std::any Timer::update() {
    if (pause_ || nodes_.empty()) {
        return {};
    }

    auto it = nodes_.begin();
    if (it != nodes_.end() && tick() >= it->expire_time) {
        std::any res = it->func(it->cnt);
        TimerNode node = std::move(*it);
        nodes_.erase(it);
        if (node.cnt == -1 || --node.cnt > 0) {
            node.expire_time = tick() + node.interval;
            nodes_.insert(node);
        }
        return res;
    }
    return {};
}

timer_t Timer::time_to_sleep() const {
    if (pause_) {
        return NO_SLEEP;
    }

    if (auto it = nodes_.begin(); it != nodes_.end()) {
        return it->expire_time >= tick() ? it->expire_time - tick() : 0;
    }
    return NO_SLEEP;
}

void Timer::sleep(timer_t us) const {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

}  // namespace wheel
