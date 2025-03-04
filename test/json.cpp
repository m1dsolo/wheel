#include <wheel/json.hpp>

#include <gtest/gtest.h>

namespace wheel {

TEST(JsonTest, Null) {
    std::nullptr_t ptr = Json::parse("null");
    EXPECT_EQ(ptr, nullptr);
}

TEST(JsonTest, Bool) {
    bool val = Json::parse("true");
    EXPECT_EQ(val, true);
    val = Json::parse("false");
    EXPECT_EQ(val, false);
}

TEST(JsonTest, Int) {
    int val = Json::parse("123");
    EXPECT_EQ(val, 123);
}

TEST(JsonTest, Double) {
    double val = Json::parse("3.14");
    EXPECT_EQ(val, 3.14);
}

TEST(JsonTest, String) {
    std::string s = Json::parse(R"("hello")");
    EXPECT_EQ(s, R"(hello)");
}

TEST(JsonTest, StringQuote) {
    std::string s = Json::parse(R"("\"hello\"")");
    EXPECT_EQ(s, "\"hello\"");
}

TEST(JsonTest, StringEscape) {
    std::string s = Json::parse(R"("hello\nworld")");
    EXPECT_EQ(s, "hello\nworld");
}

TEST(JsonTest, List) {
    auto json = Json::parse(R"([123, "hello"])");
    EXPECT_EQ(static_cast<int>(json[0]), 123);
    EXPECT_EQ(json[1], "hello");
}

TEST(JsonTest, Dict) {
    auto json = Json::parse(R"({"hello": 123, "world": 3.14})");
    EXPECT_EQ(static_cast<int>(json["hello"]), 123);
    EXPECT_EQ(static_cast<double>(json["world"]), 3.14);
}

TEST(JsonTest, Empty) {
    auto res = Json::parse("").empty();
    EXPECT_EQ(res, true);
}

TEST(JsonTest, Error) {
    auto res = Json::parse("123abc").empty();
    EXPECT_EQ(res, true);
}

TEST(JsonTest, Nested) {
    auto json = Json::parse(R"({"hello": {"key": null}, "world": [3.14, {"key": true}]})");
    EXPECT_EQ(static_cast<std::nullptr_t>(json["hello"]["key"]), nullptr);
    EXPECT_EQ(static_cast<double>(json["world"][0]), 3.14);
    EXPECT_EQ(static_cast<bool>(json["world"][1]["key"]), true);
}

}  // namespace wheel
