#include <wheel/json.hpp>

#include <fstream>
#include <regex>

namespace wheel {

JsonObject Json::parse(std::string_view json) {
    auto [res, len] = parse_(json);
    if (len != json.length())
        return {};
    return res;
}

JsonObject Json::parse_file(const std::string& json_file_name) {
    std::ifstream file(json_file_name, std::ios::in);
    if (!file.good()) {
        return {};
    }

    std::string json, line;
    while (std::getline(file, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        json += line;
    }

    return wheel::Json::parse(json);
}

void Json::save(const JsonObject& obj, const std::string& json_file_name) {
    std::ofstream file(json_file_name, std::ios::out);
    if (!file.good()) {
        return;
    }
    file << obj;
}

const std::unordered_map<char, char> Json::unescaped_chars_map = {
    {'n', '\n'},
    {'r', '\r'},
    {'t', '\t'},
    {'a', '\a'},
    {'b', '\b'},
    {'f', '\f'},
    {'v', '\v'},
    {'0', '\0'}
};

std::pair<JsonObject, size_t> Json::parse_(std::string_view json) {
    if (json.empty()) {
        return {{}, 0};
    // remove leading space
    } else if (size_t i = json.find_first_not_of(" \n\r\t\v\f\0")) {
        auto [obj, j] = parse_(json.substr(i));
        return {std::move(obj), i + j};
    } else if (json[0] == 'n') {
        return parse_null_(json);
    } else if (json[0] == 't' || json[0] == 'f') {
        return parse_bool_(json);
    } else if ((json[0] >= '0' && json[0] <= '9') || (json[0] == '+' || json[0] == '-')) {
        return parse_num_(json);
    } else if (json[0] == '"') {
        return parse_str_(json);
    } else if (json[0] == '[') {
        return parse_list_(json);
    } else if (json[0] == '{') {
        return parse_dict_(json);
    }
    return {{}, 0};
}

std::pair<JsonObject, size_t> Json::parse_null_(std::string_view json) {
    if (json.substr(0, 4) == "null")
        return {JsonObject{nullptr}, 4};
    return {{}, 0 };
}

std::pair<JsonObject, size_t> Json::parse_bool_(std::string_view json) {
    if (json.substr(0, 4) == "true")
        return {JsonObject{true}, 4};
    else if (json.substr(0, 5) == "false")
        return {JsonObject{false}, 5};
    return {{}, 0};
}

std::pair<JsonObject, size_t> Json::parse_num_(std::string_view json) {
    std::regex num_pattern {R"([+-]?\d+(\.\d+)?([eE][+-]?\d+)?)"};
    std::cmatch match;
    if (std::regex_search(json.begin(), json.end(), match, num_pattern)) {
        std::string str = match.str();
        if (auto num = try_parse_num_<int>(str)) {
            return {JsonObject{*num}, str.length()};
        } else if (auto num = try_parse_num_<double>(str)) {
            return {JsonObject{*num}, str.length()};
        }
    }
    return {JsonObject{}, 0};
}

std::pair<JsonObject, size_t> Json::parse_str_(std::string_view json) {
    // handle transcode
    std::string str;
    bool raw = true;
    size_t i = 1;
    while (i < json.length()) {
        char c = json[i++];
        if (raw) {
            if (c == '\\') {
                raw = false;
            } else if (c == '"') {
                break;
            } else {
                str += c;
            }
        } else {
            str += decode_unescaped_char_(c);
            raw = true;
        }
    }
    return {JsonObject{std::move(str)}, i};
}

std::pair<JsonObject, size_t> Json::parse_list_(std::string_view json) {
    JsonListType res;
    size_t i = 1;
    while (i < json.length()) {
        if (json[i] == ']') {
            ++i;
            break;
        }
        auto [obj, j] = parse_(json.substr(i));
        if (j == 0) {  // if failed
            i = 0;
            break;
        }
        res.push_back(std::move(obj));
        i += j;
        if (json[i] == ',')
            ++i;
    }
    return {JsonObject{std::move(res)}, i};
}

std::pair<JsonObject, size_t> Json::parse_dict_(std::string_view json) {
    JsonDictType res;
    size_t i = 1;
    while (i < json.length()) {
        if (json[i] == '}') {
            ++i;
            break;
        }
        // key
        auto [key_obj, j] = parse_(json.substr(i));
        if (j == 0) {
            i = 0;
            break;
        }
        i += j;
        if (!std::holds_alternative<std::string>(key_obj.value_)) {
            i = 0;
            break;
        }
        if (json[i] == ':')
            ++i;
        
        // value
        auto [val_obj, k] = parse_(json.substr(i));
        if (k == 0) {
            i = 0;
            break;
        }
        i += k;
        res.insert_or_assign(std::move(std::get<std::string>(key_obj.value_)), std::move(val_obj));
        if (json[i] == ',')
            ++i;
    }
    return {JsonObject{std::move(res)}, i};
}

char Json::decode_unescaped_char_(char c) {
    return unescaped_chars_map.count(c) ? unescaped_chars_map.at(c) : c;
}

std::ostream& operator<<(std::ostream& os, const JsonObject& obj) {
    std::visit([&](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            os << "error";
        } else if constexpr (std::is_same_v<T, bool>) {
            os << std::boolalpha << v << std::noboolalpha;
        } else if constexpr (std::is_same_v<T, int>) {
            os << v;
        } else if constexpr (std::is_same_v<T, double>) {
            os << v;
        } else if constexpr (std::is_same_v<T, std::string>) {
            os << '"' << v << '"';
        } else if constexpr (std::is_same_v<T, JsonListType>) {
            os << '[';
            for (size_t i = 0; i < v.size(); ++i) {
                os << v[i];
                if (i < v.size() - 1) {
                    os << ", ";
                }
            }
            os << ']';
        } else if constexpr (std::is_same_v<T, JsonDictType>) {
            os << '{';
            bool first = true;
            for (const auto& [key, value] : v) {
                if (!first) {
                    os << ", ";
                } else {
                    first = false;
                }
                os << '"' << key << "\": " << value;
            }
            os << '}';
        }
    }, obj.value_);
    return os;
}

}  // namespace wheel
