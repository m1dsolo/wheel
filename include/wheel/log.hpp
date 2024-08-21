// TODO: 异步
// TODO: 日志文件滚动（跨天或者文件大于一定大小，会建立新文件）
// TODO: 考虑程序崩溃
#pragma once

#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <source_location>

#include <wheel/enum.hpp>

namespace wheel {

enum class LogLevel : uint8_t {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

class Log {
private:
    template <typename T>
    class with_source_location_;

public:
    template <typename ...Args>
    static void log(LogLevel level, with_source_location_<std::format_string<Args...>> fmt, Args&& ...args);
    template <typename ...Args>
    static void debug(with_source_location_<std::format_string<Args...>> fmt, Args&& ...args) {
        return log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
    }
    template <typename ...Args>
    static void info(with_source_location_<std::format_string<Args...>> fmt, Args&& ...args) {
        return log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
    }
    template <typename ...Args>
    static void warning(with_source_location_<std::format_string<Args...>> fmt, Args&& ...args) {
        return log(LogLevel::WARNING, fmt, std::forward<Args>(args)...);
    }
    template <typename ...Args>
    static void error(with_source_location_<std::format_string<Args...>> fmt, Args&& ...args) {
        return log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
    }
    template <typename ...Args>
    static void fatal(with_source_location_<std::format_string<Args...>> fmt, Args&& ...args) {
        return log(LogLevel::FATAL, fmt, std::forward<Args>(args)...);
    }
    template <typename ...Args>
    static void assert_(bool condition, with_source_location_<std::format_string<Args...>> fmt, Args&& ...args) {
        if (!condition) {
            log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
            exit(1);
        }
    }

    template <typename T>
    static void log(LogLevel level, const T& val, std::source_location loc = std::source_location::current());
    template <typename T>
    static void debug(const T& val, std::source_location loc = std::source_location::current()) {
        return log(LogLevel::DEBUG, val, loc);
    }
    template <typename T>
    static void info(const T& val, std::source_location loc = std::source_location::current()) {
        return log(LogLevel::INFO, val, loc);
    }
    template <typename T>
    static void warning(const T& val, std::source_location loc = std::source_location::current()) {
        return log(LogLevel::WARNING, val, loc);
    }
    template <typename T>
    static void error(const T& val, std::source_location loc = std::source_location::current()) {
        return log(LogLevel::ERROR, val, loc);
    }
    template <typename T>
    static void fatal(const T& val, std::source_location loc = std::source_location::current()) {
        return log(LogLevel::FATAL, val, loc);
    }
    template <typename T>
    static void assert_(bool condition, const T& val, std::source_location loc = std::source_location::current()) {
        if (!condition) {
            log(LogLevel::ERROR, val, loc);
            exit(1);
        }
    }
    template <typename F> requires std::invocable<F> && std::convertible_to<std::invoke_result_t<F>, std::string>
    static void assert_(bool condition, const F& f, std::source_location loc = std::source_location::current()) {
        if (!condition) {
            log(LogLevel::ERROR, f(), loc);
            exit(1);
        }
    }

    static void set_log_level(LogLevel level) { max_log_level_ = level; }
    static void set_log_file(std::string_view s) { log_file_.open(s.data(), std::ios::app); }

private:
    template <typename T>
    class with_source_location_ {
    public:
        template <typename U> requires std::constructible_from<T, U>
        consteval with_source_location_(U&& u, std::source_location loc = std::source_location::current())
            : inner(std::forward<U>(u)), loc(loc) {}
        constexpr const T& format() const { return inner; }
        constexpr const std::source_location& location() const { return loc; }

    private:
        T inner;
        std::source_location loc;
    };

    static void log_(LogLevel level, std::string_view sv, const std::source_location& loc);

    [[nodiscard]]
    static std::string add_color_to_string_(LogLevel level, const std::string& s, std::string_view color);

    static LogLevel max_log_level_;
    static std::ofstream log_file_;
    static constexpr char level_ansi_color_[(uint8_t)LogLevel::FATAL + 1][6] {
        "35m",  // DEBUG, pink
        "32m",  // INFO, green
        "33m",  // WARNING, yellow
        "31m",  // ERROR, red
        "31;1m",  // FATAL, bold red
    };
};

template <typename ...Args>
void Log::log(LogLevel level, with_source_location_<std::format_string<Args...>> fmt, Args&& ...args) {
    const auto& loc = fmt.location();
    return log_(level, std::vformat(fmt.format().get(), std::make_format_args(args...)), loc);
}

template <typename T>
void Log::log(LogLevel level, const T& val, std::source_location loc) {
    return log_(level, std::format("{}", val), loc);
}

}  // namespace wheel
