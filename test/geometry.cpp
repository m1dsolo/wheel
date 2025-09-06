#include <wheel/geometry.hpp>

#include <gtest/gtest.h>

TEST(Vector2DTest, DivScalar) {
    wheel::Vector2D<float> v1{1, 2};
    wheel::Vector2D<float> v2{0.5, 1};
    EXPECT_EQ(v1 / 2, v2);
}

TEST(Vector2DTest, RatioClamp) {
    wheel::Vector2D<float> vec1(10.0f, 5.0f);
    vec1.ratio_clamp_(2.0f, 8.0f);
    assert(vec1.x == 8.0f);
    assert(vec1.y == 4.0f);

    wheel::Vector2D<float> vec2(1.0f, 2.0f);
    vec2.ratio_clamp_(2.0f, 8.0f);
    assert(vec2.x == 2.0f);
    assert(vec2.y == 4.0f);

    wheel::Vector2D<float> vec3(6.0f, 3.0f);
    vec3.ratio_clamp_(2.0f, 8.0f);
    assert(vec3.x == 6.0f);
    assert(vec3.y == 3.0f);

    wheel::Vector2D<float> vec4(9.0f, 3.0f);
    vec4.ratio_clamp_(2.0f, 8.0f);
    assert(vec4.x == 8.0f);
    assert(vec4.y == 2.66667f);
}
