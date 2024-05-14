#include <wheel/json.hpp>

#include <gtest/gtest.h>

namespace wheel {

TEST(JsonParseTest, ParseNull) {
    auto res = Json::parse("null").get<std::nullptr_t>();
    EXPECT_EQ(res, nullptr);
}

TEST(JsonParseTest, ParseBool) {
    auto res = Json::parse("true").get<bool>();
    EXPECT_EQ(res, true);
    res = Json::parse("false").get<bool>();
    EXPECT_EQ(res, false);
}

TEST(JsonParseTest, ParseInt) {
    auto res = Json::parse("123").get<int>();
    EXPECT_EQ(res, 123);
}

TEST(JsonParseTest, ParseDouble) {
    auto res = Json::parse("3.14").get<double>();
    EXPECT_EQ(res, 3.14);
}

TEST(JsonParseTest, ParseString) {
    auto res = Json::parse(R"("hello")").get<std::string>();
    EXPECT_EQ(res, R"(hello)");
}

TEST(JsonParseTest, ParseStringQuote) {
    auto res = Json::parse(R"("\"hello\"")").get<std::string>();
    EXPECT_EQ(res, "\"hello\"");
}

TEST(JsonParseTest, ParseStringEscape) {
    auto res = Json::parse(R"("hello\nworld")").get<std::string>();
    EXPECT_EQ(res, "hello\nworld");
}

TEST(JsonParseTest, ParseList) {
    auto res = Json::parse(R"([123, "hello"])").get<JsonListType>();
    EXPECT_EQ(res[0].get<int>(), 123);
    EXPECT_EQ(res[1].get<std::string>(), "hello");
}

TEST(JsonParseTest, ParseDict) {
    auto res = Json::parse(R"({"hello": 123, "world": 3.14})").get<JsonDictType>();
    EXPECT_EQ(res["hello"].get<int>(), 123);
    EXPECT_EQ(res["world"].get<double>(), 3.14);
}

TEST(JsonParseTest, ParseEmpty) {
    auto res = Json::parse("").get<std::monostate>();
    EXPECT_EQ(res, std::monostate());
}

TEST(JsonParseTest, ParseError) {
    auto res = Json::parse("123abc").get<std::monostate>();
    EXPECT_EQ(res, std::monostate());
}

TEST(JsonParseTest, ParseNested) {
    auto res = Json::parse(R"({"hello": {"key": null}, "world": [3.14, {"key": true}]})").get<JsonDictType>();
    auto mp = res["hello"].get<JsonDictType>();
    EXPECT_EQ(mp["key"].get<std::nullptr_t>(), nullptr);
    auto list = res["world"].get<JsonListType>();
    EXPECT_EQ(list[0].get<double>(), 3.14);
    mp = list[1].get<JsonDictType>();
    EXPECT_EQ(mp["key"].get<bool>(), true);
}

}  // namespace wheel
