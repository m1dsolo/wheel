// When the data are consecutive numbers, SparseSet is faster than unordered_set.
// because of cpu cache and hash collision.

#pragma once

#include <type_traits>
#include <vector>
#include <unordered_map>

namespace wheel {

template <typename T> requires std::is_integral_v<T>
class SparseSet final {
public:
    SparseSet() {}
    explicit SparseSet(size_t size) {
        dense_.reserve(size);
    }
    ~SparseSet() = default;

    void add(const T& val) {
        dense_.emplace_back(val);
        sparse_[val] = dense_.size() - 1;
    }

    void del(const T& val) {
        if (!sparse_.count(val)) return;

        size_t idx = sparse_[val];
        if (val != dense_.back()) {
            dense_[idx] = dense_.back();
            sparse_[dense_.back()] = idx;
        }
        dense_.pop_back();
        sparse_.erase(val);
    }

    bool has(const T& val) const {
        return sparse_.count(val);
    }

    const auto begin() const { return dense_.begin(); }
    const auto end() const { return dense_.end(); }

    const std::vector<T>& entities() const { return dense_; }

private:
    std::vector<T> dense_;
    std::unordered_map<T, size_t> sparse_;
};

}  // namespace wheel
