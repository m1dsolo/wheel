#pragma once

#include <cstdint>
#include <string>

namespace wheel {

constexpr uint32_t FNV1A_32_OFFSET = 0x811c9dc5;
constexpr uint32_t FNV1A_32_PRIME = 0x01000193;

struct ID {
    uint32_t val = FNV1A_32_OFFSET;

    constexpr ID() {}
    constexpr ID(uint32_t val) : val(val) {}
    ID(const std::string& str);

    template <std::size_t N>
    constexpr ID(const char(&str)[N]);

    operator uint32_t() const { return val; }

    constexpr bool operator==(const ID other) const { return val == other.val; }
    constexpr ID operator^(uint32_t other) const { return ID(val ^ other); }
    constexpr ID operator*(uint32_t other) const { return ID(val * other); }
};

namespace detail {

constexpr ID id_of(const char* str, std::size_t len, ID id = FNV1A_32_OFFSET) {
    return (len == 0) ? id : id_of(str + 1, len - 1, (id ^ static_cast<ID>(*str)) * FNV1A_32_PRIME);
}

}

template <std::size_t N>
constexpr ID::ID(const char(&str)[N]) {
    val = wheel::detail::id_of(str, N - 1);
}

}  // namespace wheel

namespace std {
    template<>
    struct hash<wheel::ID> {
        size_t operator()(const wheel::ID& id) const noexcept {
            return id.val;
        }
    };
}  // namespace std
