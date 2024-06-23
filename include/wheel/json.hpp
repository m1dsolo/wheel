// TODO: null true false
// TODO: string同时支持单引号和双引号
// TODO: map key支持非引号
// TODO: pretty print
// TODO: hex \x0D
// TODO: 判断合法
// TODO: 递归改为用栈
// TODO: get防止报错

#pragma once

#include <iostream>
#include <charconv>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_map>
#include <optional>

namespace wheel {

class JsonObject;
using JsonListType = std::vector<JsonObject>;
using JsonDictType = std::unordered_map<std::string, JsonObject>;
using JsonValueType = std::variant<
    std::monostate, // error
    std::nullptr_t, // null
    bool,           // true
    int,            // 123
    double,         // 3.14
    std::string,    // "hello"
    JsonListType,   // [123, "hello"]
    JsonDictType    // {"hello": 123, "world": 3.14}
>;

class JsonObject {
public:
    JsonObject() : value_(std::monostate()) {}
    explicit JsonObject(const JsonValueType& value) : value_(value) {}

    explicit operator std::nullptr_t() const { return get<std::nullptr_t>(); }
    explicit operator bool() const { return get<bool>(); }
    explicit operator int() const { return get<int>(); }
    explicit operator double() const { return get<double>(); }
    explicit operator std::string&() { return get<std::string>(); }
    explicit operator JsonListType&() { return get<JsonListType>(); }
    explicit operator JsonDictType&() { return get<JsonDictType>(); }
    explicit operator const std::string&() const { return get<std::string>(); }
    explicit operator const JsonListType&() const { return get<JsonListType>(); }
    explicit operator const JsonDictType&() const { return get<JsonDictType>(); }
    JsonObject& operator[](size_t i) { return get<JsonListType>()[i]; }
    JsonObject& operator[](const std::string& key) { return get<JsonDictType>()[key]; }
    const JsonObject& operator[](size_t i) const { return get<JsonListType>()[i]; }
    const JsonObject& operator[](const std::string& key) const { return get<JsonDictType>().at(key); }

    bool empty() const { return value_.index() == 0; }

private:
    friend class Json;
    friend std::ostream& operator<<(std::ostream& os, const JsonObject& obj);

    template<typename T>
    T& get() { return std::get<T>(value_); }
    template<typename T>
    const T& get() const { return std::get<T>(value_); }

    JsonValueType value_;
};

class Json {
public:
    static JsonObject parse(std::string_view json);
    static JsonObject parse_file(const std::string& json_file_name);

private:
    static std::pair<JsonObject, size_t> parse_(std::string_view json);
    static std::pair<JsonObject, size_t> parse_null_(std::string_view json);
    static std::pair<JsonObject, size_t> parse_bool_(std::string_view json);
    static std::pair<JsonObject, size_t> parse_num_(std::string_view json);
    static std::pair<JsonObject, size_t> parse_str_(std::string_view json);
    static std::pair<JsonObject, size_t> parse_list_(std::string_view json);
    static std::pair<JsonObject, size_t> parse_dict_(std::string_view json);

    template<typename T>
    static std::optional<T> try_parse_num_(std::string_view str) {
        T res;
        auto ret = std::from_chars(str.data(), str.data() + str.length(), res);
        if (ret.ec == std::errc() && ret.ptr == str.data() + str.length()) {
            return res;
        }
        return std::nullopt;
    }
    static char decode_unescaped_char_(char c);

    static const std::unordered_map<char, char> unescaped_chars_map;
};

}  // namespace wheel
