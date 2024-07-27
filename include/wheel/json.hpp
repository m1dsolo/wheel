// TODO: null true false
// TODO: string同时支持单引号和双引号
// TODO: map key支持非引号
// TODO: pretty print
// TODO: hex \x0D
// TODO: 判断合法
// TODO: 递归改为用栈
// TODO: get防止报错
// TODO: reflect save
// TODO: vector<int> v = json_obj(递归get？)

#pragma once

#include <charconv>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>

namespace wheel {

class JsonObject;
using JsonListType = std::vector<JsonObject>;
using JsonDictType = std::map<std::string, JsonObject>;
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
    JsonObject() {}
    JsonObject(const JsonValueType& value) : value_(value) {}
    JsonObject(JsonValueType&& value) : value_(std::move(value)) {}
    JsonObject& operator=(const JsonObject& value) {
        value_ = value.value_;
        return *this;
    }
    // JsonObject hasn't full defination, so maybe can't overload JsonObject& operator=(JsonObject&& value)
    // JsonObject& operator=(JsonObject&& value) {
    //     *this = std::move(value.value_);
    //     return *this;
    // }

    // operator std::nullptr_t() const { return get<std::nullptr_t>(); }
    // operator bool() const { return get<bool>(); }
    // operator int() const { return get<int>(); }
    // operator double() const { return get<double>(); }
    // operator std::string&() { return get<std::string>(); }
    // // operator std::string_view() { return get<std::string>(); }
    // operator JsonListType&() { return get<JsonListType>(); }
    // operator JsonDictType&() { return get<JsonDictType>(); }

    // template <typename T>
    // operator T() { return get<T>(); }

    template <typename T>
    operator T&() { return get<T>(); }
    // operator const std::string&() const { return get<std::string>(); }
    // // operator const std::string_view() const { return get<std::string>(); }
    // operator const JsonListType&() const { return get<JsonListType>(); }
    // operator const JsonDictType&() const { return get<JsonDictType>(); }
    template <typename T>
    operator const T&() const { return get<T>(); }

    template <typename T> requires std::is_integral<T>::value
    JsonObject& operator[](T i) { return get<JsonListType>()[i]; }

    template <typename T> requires std::is_integral<T>::value
    const JsonObject& operator[](T i) const { return get<JsonListType>()[i]; }

    // JsonObject& operator[](size_t i) { return get<JsonListType>()[i]; }
    JsonObject& operator[](const char* key) { return get<JsonDictType>()[key]; }
    JsonObject& operator[](const std::string& key) { return get<JsonDictType>()[key]; }
    // const JsonObject& operator[](size_t i) const { return get<JsonListType>()[i]; }
    const JsonObject& operator[](const char* key) const { return get<JsonDictType>().at(key); }
    const JsonObject& operator[](const std::string& key) const { return get<JsonDictType>().at(key); }

    // template <typename T>
    // JsonObject& operator[](const T& key) { return get<JsonDictType>()[key]; }

    // for const char* compare
    bool operator==(const char* str) const { return get<std::string>() == str; }
    bool operator!=(const char* str) const { return get<std::string>() != str; }
    friend bool operator==(const char* str, const JsonObject& obj) { return obj.get<std::string>() == str; }
    friend bool operator!=(const char* str, const JsonObject& obj) { return obj.get<std::string>() != str; }

    // auto begin() { return get<JsonDictType>().begin(); }
    // auto end() { return get<JsonDictType>().end(); }

    bool empty() const { return value_.index() == 0; }

    JsonValueType value_ = std::monostate{};

    template <typename T>
    T& get() { return std::get<T>(value_); }
    template <typename T>
    const T& get() const { return std::get<T>(value_); }

private:
    friend class Json;
    friend std::ostream& operator<<(std::ostream& os, const JsonObject& obj);
};

class Json {
public:
    static JsonObject parse(std::string_view json);
    static JsonObject parse_file(const std::string& json_file_name);

    static void save(const JsonObject& obj, const std::string& json_file_name);

    template <typename T>
    static T get(const JsonObject& obj) {
        return std::get<T>(obj);
    }

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
