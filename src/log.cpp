#include <wheel/log.hpp>

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

std::string Log::add_color_to_string_(LogLevel level, const std::string& s, std::string_view color) {
    return std::format("\E[{}{}\E[m", color, s);
}

}  // namespace wheel
