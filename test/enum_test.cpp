#include <wheel/enum.hpp>

#include <gtest/gtest.h>

namespace wheel {

enum Color {
    RED, BLUE, GREEN
};

TEST(EnumTest, Enum2Str) {
    EXPECT_EQ(Enum::enum2str(RED), "RED");
    Color color = GREEN;
    EXPECT_EQ(Enum::enum2str(color), "GREEN");
    EXPECT_EQ(Enum::enum2str(100), "100");
    EXPECT_EQ((Enum::enum2str<Color, 0, 2>(color)), "GREEN");
}

TEST(EnumTest, Str2Enum) {
    EXPECT_EQ(Enum::str2enum<Color>("BLUE"), BLUE);
}

};  // namespace wheel
