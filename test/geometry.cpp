#include <wheel/geometry.hpp>

#include <gtest/gtest.h>

TEST(Vector2DTest, DivScalar) {
    wheel::Vector2D<float> v1{1, 2};
    wheel::Vector2D<float> v2{0.5, 1};
    EXPECT_EQ(v1 / 2, v2);
}
