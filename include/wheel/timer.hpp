#pragma once

#include <any>
#include <cstdint>
#include <functional>
#include <set>

#include <wheel/singleton.hpp>
#include <wheel/log.hpp>

namespace wheel {

using timer_t = uint64_t;

struct TimerNode {
    TimerNode(timer_t expire_time, timer_t interval, int cnt, std::function<std::any()> func)
        : expire_time(expire_time), interval(interval), cnt(cnt), func(func), id(gid_++) {}

    timer_t expire_time;
    timer_t interval;
    int cnt;
    std::function<std::any()> func;
    uint32_t id;

    bool operator<(const TimerNode& rhs) const {
        return (expire_time == rhs.expire_time) ? id < rhs.id : expire_time < rhs.expire_time;
    }

private:
    static inline uint32_t gid_ = 0;
};

// use microsecond as time unit
class Timer : public Singleton<Timer> {
    friend class Singleton<Timer>;

public:
    timer_t tick() const;

    // cnt: -1 for infinite
    template <typename F, typename... Args>
    void add(timer_t interval_us, int cnt, F&& f, Args&&... args) {
        Log::assert(interval_us > 0, "interval_us must be positive");
        auto func = [f = std::forward<F>(f), ...args = std::forward<Args>(args)]() -> std::any {
            if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>) {
                f(args...);
                return std::any{};
            } else {
                return f(args...);
            }
        };
        nodes_.insert(TimerNode(tick() + interval_us, interval_us, cnt, func));
    }

    void set_pause(bool pause) { pause_ = pause; }
    void pause();
    void resume();

    size_t size() const { return nodes_.size(); }

    std::any update();

    timer_t time_to_sleep() const;

    void sleep(timer_t us) const;

    static inline constexpr timer_t NO_SLEEP = (timer_t)-1;
    static inline constexpr timer_t UNITS_IN_SECOND = 1000000;

private:
    Timer() = default;
    Timer(const Timer&) = delete;

    std::set<TimerNode> nodes_;

    // for pause and resume
    bool pause_;
    timer_t pause_start_tick_;
};

}  // namespace wheel
