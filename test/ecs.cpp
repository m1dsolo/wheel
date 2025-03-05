#include <wheel/ecs.hpp>

#include <gtest/gtest.h>

using namespace wheel;

struct PositionComponent {
    float x, y;
};

struct VelocityComponent {
    float dx, dy;
};

struct HPComponent {
    int hp;
};

class ECSTest : public ::testing::Test {
protected:
    void SetUp() override {
        ecs.clear_entities();
        ecs.clear_systems();
        ecs.clear_events();
    }

    ECS ecs;
};

TEST_F(ECSTest, EntityGeneration) {
    Entity entity1 = ecs.add_entity();
    Entity entity2 = ecs.add_entity();
    
    EXPECT_NE(entity1, entity2);
    EXPECT_EQ(ecs.count_entities(), 2);
    EXPECT_TRUE(ecs.has_entity(entity1));
    EXPECT_TRUE(ecs.has_entity(entity2));
}

TEST_F(ECSTest, ComponentManagement) {
    Entity entity = ecs.add_entity(
        PositionComponent{1.0f, 2.0f},
        VelocityComponent{0.5f, 0.5f}
    );

    auto res = ecs.has_components<PositionComponent, VelocityComponent>(entity);
    EXPECT_TRUE(res);
    
    auto& position = ecs.get_component<PositionComponent>(entity);
    auto& velocity = ecs.get_component<VelocityComponent>(entity);

    EXPECT_FLOAT_EQ(position.x, 1.0f);
    EXPECT_FLOAT_EQ(position.y, 2.0f);
    EXPECT_FLOAT_EQ(velocity.dx, 0.5f);
    EXPECT_FLOAT_EQ(velocity.dy, 0.5f);
}

TEST_F(ECSTest, ComponentDeletion) {
    Entity entity = ecs.add_entity(
        PositionComponent{1.0f, 2.0f},
        VelocityComponent{0.5f, 0.5f}
    );

    ecs.del_component<PositionComponent>(entity);
    
    EXPECT_FALSE(ecs.has_component<PositionComponent>(entity));
    EXPECT_TRUE(ecs.has_component<VelocityComponent>(entity));
}

TEST_F(ECSTest, EntityDeletion) {
    Entity entity = ecs.add_entity(
        PositionComponent{1.0f, 2.0f},
        VelocityComponent{0.5f, 0.5f}
    );

    ecs.del_entity(entity);
    
    EXPECT_FALSE(ecs.has_entity(entity));
}

TEST_F(ECSTest, EntityQuery) {
    Entity entity1 = ecs.add_entity(
        PositionComponent{1.0f, 2.0f},
        HPComponent{100}
    );

    Entity entity2 = ecs.add_entity(
        PositionComponent{3.0f, 4.0f}
    );

    auto entities = ecs.get_entities<PositionComponent, HPComponent>();
    
    EXPECT_EQ(std::distance(entities.begin(), entities.end()), 1);
    EXPECT_EQ(entities.front(), entity1);
}

TEST_F(ECSTest, EventSystem) {
    struct TestEvent {
        int value;
    };

    ecs.add_event(TestEvent{42});
    ecs.update();

    EXPECT_TRUE(ecs.has_event<TestEvent>());
    
    auto events = ecs.get_events<TestEvent>();
    EXPECT_EQ(std::distance(events.begin(), events.end()), 1);
    EXPECT_EQ(events.front().value, 42);
}

TEST_F(ECSTest, SystemExecution) {
    int counter = 0;
    
    ecs.add_system( [&counter]() {
        counter++;
    });

    ecs.update();
    ecs.update();

    EXPECT_EQ(counter, 2);
}

TEST_F(ECSTest, ComponentExclusion) {
    Entity entity1 = ecs.add_entity(
        PositionComponent{1.0f, 2.0f},
        HPComponent{100}
    );

    Entity entity2 = ecs.add_entity(
        PositionComponent{3.0f, 4.0f}
    );

    ecs.exclude_component<HPComponent>();

    auto entities1 = ecs.get_entities<PositionComponent>();
    EXPECT_EQ(std::distance(entities1.begin(), entities1.end()), 1);
    EXPECT_EQ(entities1.front(), entity2);

    auto entities2 = ecs.get_entities<PositionComponent, HPComponent>();
    EXPECT_EQ(std::distance(entities2.begin(), entities2.end()), 1);
    EXPECT_EQ(entities2.front(), entity1);

    auto res = ecs.has_components<PositionComponent, HPComponent>(entity1);
    EXPECT_EQ(res, true);
    res = ecs.has_components<PositionComponent>(entity1);
    EXPECT_EQ(res, true);
    res = ecs.has_components<HPComponent>(entity1);
    EXPECT_EQ(res, true);
    res = ecs.has_components<HPComponent>(entity2);
    EXPECT_EQ(res, false);

    int size = 0;
    for (auto entity : ecs.get_entities()) {
        size++;
    }
    EXPECT_EQ(size, 2);
}
