#include <gtest/gtest.h>

#include <wheel/quadtree.hpp>

using namespace wheel;

struct Rectangle {
    float x1, y1, x2, y2;

    Rect<float> toRect() const {
        return {x1, y1, x2 - x1, y2 - y1};
    }

    bool operator==(const Rectangle& other) const {
        return std::tie(x1, y1, x2, y2) == std::tie(other.x1, other.y1, other.x2, other.y2);
    }
};

TEST(QuadTreeTest, AddRemoveAndQuery) {
    auto get_rect = [](const Rectangle& r) { return r.toRect(); };
    QuadTree<Rectangle, decltype(get_rect)> tree({0, 0, 10, 10}, get_rect, 4, 4);

    Rectangle r1 = {1, 1, 2, 2};
    Rectangle r2 = {3, 3, 4, 4};
    Rectangle r3 = {5, 5, 6, 6};
    Rectangle r4 = {7, 7, 8, 8};

    tree.add(r1);
    tree.add(r2);
    tree.add(r3);
    tree.add(r4);

    EXPECT_EQ(tree.query(r1).size(), 1);
    EXPECT_EQ(tree.query(r2).size(), 1);
    EXPECT_EQ(tree.query(r3).size(), 1);
    EXPECT_EQ(tree.query(r4).size(), 1);

    tree.remove(r2);

    EXPECT_EQ(tree.query(r2).size(), 0);
}

TEST(QuadTreeTest, FindAllIntersections) {
    auto get_rect = [](const Rectangle& r) { return r.toRect(); };
    QuadTree<Rectangle, decltype(get_rect)> tree({0, 0, 10, 10}, get_rect, 4, 4);

    Rectangle r1 = {1, 1, 2, 2};
    Rectangle r2 = {2, 2, 3, 3};
    Rectangle r3 = {3, 3, 4, 4};
    Rectangle r4 = {4, 4, 5, 5};

    tree.add(r1);
    tree.add(r2);
    tree.add(r3);
    tree.add(r4);

    auto intersections = tree.find_all_intersections();
    EXPECT_EQ(intersections.size(), 4);

    std::vector<std::pair<Rectangle, Rectangle>> expectedIntersections = {
        {r1, r2}, {r1, r3}, {r2, r3}, {r3, r4}};
    EXPECT_EQ(intersections, expectedIntersections);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
