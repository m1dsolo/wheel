#include <wheel/json.hpp>

#include <gtest/gtest.h>

namespace wheel {

TEST(JsonParseTest, ParseNull) {
    auto res = std::nullptr_t(Json::parse("null"));
    EXPECT_EQ(res, nullptr);
}

TEST(JsonParseTest, ParseBool) {
    auto res = bool(Json::parse("true"));
    EXPECT_EQ(res, true);
    res = bool(Json::parse("false"));
    EXPECT_EQ(res, false);
}

TEST(JsonParseTest, ParseInt) {
    auto res = int(Json::parse("123"));
    EXPECT_EQ(res, 123);
}

TEST(JsonParseTest, ParseDouble) {
    auto res = double(Json::parse("3.14"));
    EXPECT_EQ(res, 3.14);
}

TEST(JsonParseTest, ParseString) {
    const auto& res = std::string(Json::parse(R"("hello")"));
    EXPECT_EQ(res, R"(hello)");
}

TEST(JsonParseTest, ParseStringQuote) {
    const auto& res = std::string(Json::parse(R"("\"hello\"")"));
    EXPECT_EQ(res, "\"hello\"");
}

TEST(JsonParseTest, ParseStringEscape) {
    const auto& res = std::string(Json::parse(R"("hello\nworld")"));
    EXPECT_EQ(res, "hello\nworld");
}

TEST(JsonParseTest, ParseList) {
    const auto& res = Json::parse(R"([123, "hello"])");
    EXPECT_EQ(int(res[0]), 123);
    EXPECT_EQ(std::string(res[1]), "hello");
}

TEST(JsonParseTest, ParseDict) {
    const auto& res = Json::parse(R"({"hello": 123, "world": 3.14})");
    EXPECT_EQ(int(res["hello"]), 123);
    EXPECT_EQ(double(res["world"]), 3.14);
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
    const auto& res = Json::parse(R"({"hello": {"key": null}, "world": [3.14, {"key": true}]})");
    EXPECT_EQ(std::nullptr_t(res["hello"]["key"]), nullptr);
    EXPECT_EQ(double(res["world"][0]), 3.14);
    EXPECT_EQ(bool(res["world"][1]["key"]), true);
}

}  // namespace wheel
