#include <wheel/json.hpp>

#include <gtest/gtest.h>

namespace wheel {

TEST(JsonParseTest, ParseNull) {
    std::nullptr_t ptr = Json::parse("null");
    EXPECT_EQ(ptr, nullptr);
}

TEST(JsonParseTest, ParseBool) {
    bool val = Json::parse("true");
    EXPECT_EQ(val, true);
    val = Json::parse("false");
    EXPECT_EQ(val, false);
}

TEST(JsonParseTest, ParseInt) {
    int val = Json::parse("123");
    EXPECT_EQ(val, 123);
}

TEST(JsonParseTest, ParseDouble) {
    double val = Json::parse("3.14");
    EXPECT_EQ(val, 3.14);
}

TEST(JsonParseTest, ParseString) {
    std::string s = Json::parse(R"("hello")");
    EXPECT_EQ(s, R"(hello)");
}

TEST(JsonParseTest, ParseStringQuote) {
    std::string s = Json::parse(R"("\"hello\"")");
    EXPECT_EQ(s, "\"hello\"");
}

TEST(JsonParseTest, ParseStringEscape) {
    std::string s = Json::parse(R"("hello\nworld")");
    EXPECT_EQ(s, "hello\nworld");
}

TEST(JsonParseTest, ParseList) {
    auto json = Json::parse(R"([123, "hello"])");
    EXPECT_EQ(static_cast<int>(json[0]), 123);
    EXPECT_EQ(json[1], "hello");
}

TEST(JsonParseTest, ParseDict) {
    auto json = Json::parse(R"({"hello": 123, "world": 3.14})");
    EXPECT_EQ(static_cast<int>(json["hello"]), 123);
    EXPECT_EQ(static_cast<double>(json["world"]), 3.14);
}

TEST(JsonParseTest, ParseEmpty) {
    auto res = Json::parse("").empty();
    EXPECT_EQ(res, true);
}

TEST(JsonParseTest, ParseError) {
    auto res = Json::parse("123abc").empty();
    EXPECT_EQ(res, true);
}

TEST(JsonParseTest, ParseNested) {
    auto json = Json::parse(R"({"hello": {"key": null}, "world": [3.14, {"key": true}]})");
    EXPECT_EQ(static_cast<std::nullptr_t>(json["hello"]["key"]), nullptr);
    EXPECT_EQ(static_cast<double>(json["world"][0]), 3.14);
    EXPECT_EQ(static_cast<bool>(json["world"][1]["key"]), true);
}

}  // namespace wheel
