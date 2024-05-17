// TODO: handle exceptions
#pragma once

#include <string>

namespace wheel {

class Enum {
public:
    template <typename T, int Begin = 0, int End = 256>
    static std::string enum2str(T val);

    template <typename T, int Begin = 0, int End = 256>
    static T str2enum(const std::string& s);

private:
    template <typename T, T N>
    static std::string enum2str_static_();

    template <int Begin = 0, int End = 256, typename F>
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
    if (res == "")
        return std::to_string((int)val);
    return res;
}

template <typename T, int Begin, int End>
T Enum::str2enum(const std::string& s) {
    for (int i = Begin; i <= End; i++) {
        if (s == enum2str((T)i))
            return (T)i;
    }
    throw;
}

template <typename T, T N>
std::string Enum::enum2str_static_() {
    std::string s = __PRETTY_FUNCTION__;
    auto l = s.find("N = ") + 4;
    auto r = s.find_first_of(";]", l);
    s = s.substr(l, r - l);
    auto i = s.find("::");
    if (i != s.npos)
        s = s.substr(i + 2);
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

};  // namespace wheel