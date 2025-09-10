#pragma once

#include <cstdint>
#include <functional>
#include <queue>
#include <unordered_set>
#include <cassert>
#include <optional>

namespace wheel {

using time_t = int64_t;
using timer_id_t = uint32_t;
using timer_func_t = std::function<time_t()>;

// use microsecond as time unit
class Timer {
public:
    Timer() = default;

    timer_id_t add(time_t interval_us, timer_func_t func);
    void remove(timer_id_t id);

    time_t tick() const;
    void pause();
    void resume();
    size_t size() const { return nodes_.size(); }
    std::optional<timer_id_t> update();
    time_t time_util_next() const;
    void sleep(time_t us) const;

    inline static constexpr time_t TIME_UNIT_PER_SECOND = 1000000;

private:
    struct Node;

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> nodes_;
    std::unordered_set<timer_id_t> del_ids_;

    // for pause and resume
    bool pause_ = false;
    time_t pause_start_tick_;
};

struct Timer::Node {
    Node(time_t expire_time, timer_func_t func)
        : expire_time(expire_time), func(func), id(gid_++) {}

    time_t expire_time;
    std::function<time_t()> func;
    uint32_t id;

    bool operator>(const Node& rhs) const {
        return (expire_time == rhs.expire_time) ? id > rhs.id : expire_time > rhs.expire_time;
    }

private:
    static inline uint32_t gid_ = 0;
};

}  // namespace wheel
