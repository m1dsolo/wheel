#include <wheel/log.hpp>

#include <chrono>

namespace wheel {

LogLevel Log::max_log_level_ = []() {
    const char* log_level = std::getenv("LOG_LEVEL");
    if (log_level == nullptr) {
        return LogLevel::INFO;
    }
    return Enum::str2enum<LogLevel, 0, 4>(log_level).value_or(LogLevel::INFO);
} ();

std::ofstream Log::log_file_ = []() {
    if (auto log_file_name = std::getenv("LOG_FILE")) {
        return std::ofstream{log_file_name, std::ios::app};
    }
    return std::ofstream{};
} ();

void Log::log_(LogLevel level, std::string_view sv, const std::source_location& loc) {
    std::chrono::zoned_time now{
        std::chrono::current_zone(),
        std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now())
    };

    auto msg = std::format("{} [{}] {}:{} {}", 
                             now,
                             Enum::enum2str<LogLevel, 0, 4>(level),
                             loc.file_name(),
                             loc.line(),
                             sv
                           );
    if (level >= max_log_level_) {
        std::cout << add_color_to_string_(level, msg, level_ansi_color_[(uint8_t)level]) << std::endl;
    }
    if (log_file_.is_open()) {
        log_file_ << msg << std::endl;
    }
}

std::string Log::add_color_to_string_(LogLevel level, const std::string& s, std::string_view color) {
    return std::format("\E[{}{}\E[m", color, s);
}

}  // namespace wheel
