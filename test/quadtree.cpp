#include <gtest/gtest.h>

#include <wheel/quadtree.hpp>
#include <wheel/ecs.hpp>
#include <wheel/geometry.hpp>

using namespace wheel;

TEST(QuadTreeTest, AddRemoveAndQuery) {
    auto& ecs = ECS::instance();
    auto get_rect = [&](Entity entity) { return ecs.get_component<Rect<float>>(entity); };
    QuadTree<Entity, decltype(get_rect)> tree({0, 0, 10, 10}, get_rect, 4, 4);

    auto e1 = ecs.add_entity(Rect<float>{1, 1, 2, 2});
    auto e2 = ecs.add_entity(Rect<float>{3, 3, 4, 4});
    auto e3 = ecs.add_entity(Rect<float>{5, 5, 6, 6});
    auto e4 = ecs.add_entity(Rect<float>{7, 7, 8, 8});

    tree.add(e1);
    tree.add(e2);
    tree.add(e3);
    tree.add(e4);

    EXPECT_EQ(tree.query(e1).size(), 1);
    EXPECT_EQ(tree.query(e2).size(), 1);
    EXPECT_EQ(tree.query(e3).size(), 1);
    EXPECT_EQ(tree.query(e4).size(), 1);

    tree.remove(e2);

    EXPECT_EQ(tree.query(e2).size(), 0);
}

TEST(QuadTreeTest, FindAllIntersections) {
    auto& ecs = ECS::instance();
    auto get_rect = [&](Entity entity) { return ecs.get_component<Rect<float>>(entity); };
    QuadTree<Entity, decltype(get_rect)> tree({0, 0, 10, 10}, get_rect, 4, 4);

    auto e1 = ecs.add_entity(Rect<float>{1, 1, 5, 5});
    auto e2 = ecs.add_entity(Rect<float>{3, 3, 4, 4});
    auto e3 = ecs.add_entity(Rect<float>{4, 4, 6, 6});
    auto e4 = ecs.add_entity(Rect<float>{7, 7, 8, 8});

    tree.add(e1);
    tree.add(e2);
    tree.add(e3);
    tree.add(e4);

    auto intersections = tree.find_all_intersections();
    EXPECT_EQ(intersections.size(), 2);  // {e1, e2}, {e1, e3}
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
