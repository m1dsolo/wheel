// TODO: handle exceptions
#pragma once

#include <optional>
#include <source_location>
#include <string>
#include <vector>

namespace wheel {

class Enum {
public:
    template <typename T, int Begin = 0, int End = 255>
    static std::string enum2str(T val);

    template <typename T, int Begin = 0, int End = 255>
    static std::optional<T> str2enum(const std::string& s);

    template <typename T, int Begin = 0, int End = 255>
    static std::string enum_class2str(T val);

private:
    template <typename T, T N>
    static std::string enum2str_static_();

    template <typename T, T N>
    static std::string enum_class2str_static_();

    template <int Begin = 0, int End = 255, typename F>
    static void static_for_(const F& f);

};

template <typename T, int Begin, int End>
std::string Enum::enum2str(T val) {
    std::string res;
    static_for_<Begin, End>([&](auto ic) {
        constexpr int i = ic.value;
        if (val == (T)i)
            res = enum2str_static_<T, (T)i>();
    });
    if (res == "") {
        return std::to_string((int)val);
    }
    return res;
}

template <typename T, int Begin, int End>
std::optional<T> Enum::str2enum(const std::string& s) {
    for (int i = Begin; i <= End; i++) {
        if (s == enum2str((T)i)) {
            return (T)i;
        }
    }
    return std::nullopt;
}

template <typename T, int Begin, int End>
std::string Enum::enum_class2str(T val) {
    std::string res;
    static_for_<Begin, End>([&](auto ic) {
        constexpr int i = ic.value;
        if (val == (T)i)
            res = enum_class2str_static_<T, (T)i>();
    });
    if (res == "") {
        return std::to_string((int)val);
    }
    return res;
}

template <typename T, T N>
std::string Enum::enum2str_static_() {
    std::string s = std::source_location::current().function_name();
    auto l = s.find("N = ") + 4;
    auto r = s.find_first_of(";]", l);
    s = s.substr(l, r - l);
    auto i = s.find_last_of("::");
    if (i != s.npos) {
        s = s.substr(i + 1);
    }
    return s;
}

template <typename T, T N>
std::string Enum::enum_class2str_static_() {
    std::string s = std::source_location::current().function_name();
    auto l = s.find("N = ") + 4;
    auto r = s.find_first_of(";]", l);
    s = s.substr(l, r - l);

    size_t pos = 0;
    std::vector<size_t> poses;
    while ((pos = s.find("::", pos)) != std::string::npos) {
        poses.emplace_back(pos);
        pos += 2;
    }

    int n = poses.size();
    if (n == 1) {
        s = s.substr(0, poses[0]);
    } else {
        s = s.substr(poses[n - 2] + 2, poses[n - 1] - poses[n - 2] - 2);
    }

    return s;
}

template <int Begin, int End, typename F>
void Enum::static_for_(const F& f) {
    if constexpr (Begin > End) {
        return;
    } else {
        f(std::integral_constant<int, Begin>{});
        static_for_<Begin + 1, End>(f);
    }
}

}  // namespace wheel
