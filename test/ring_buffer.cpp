#include <wheel/ring_buffer.hpp>

#include <gtest/gtest.h>

class RingBufferTest : public ::testing::Test {
protected:
    wheel::RingBuffer<int, 4> buffer;
};

TEST_F(RingBufferTest, PutAndGet) {
    std::array<int, 3> data = {1, 2, 3};
    buffer.put(data, data.size());
    EXPECT_EQ(buffer.size(), 3);
    std::array<int, 3> out_data;
    buffer.get(out_data, out_data.size());
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(out_data[0], 1);
    EXPECT_EQ(out_data[1], 2);
    EXPECT_EQ(out_data[2], 3);
}

TEST_F(RingBufferTest, FullPutAndGet) {
    std::array<int, 3> data = {1, 2, 3};
    buffer.put(data, data.size());
    buffer.put(data, data.size());
    EXPECT_EQ(buffer.size(), 4);
    std::array<int, 4> out_data;
    buffer.get(out_data, out_data.size());
    EXPECT_EQ(out_data[3], 1);
}

TEST_F(RingBufferTest, WrapAroundPutAndGet) {
    std::array<int, 3> data = {1, 2, 3};
    buffer.put(data, data.size());
    std::array<int, 4> out_data;
    buffer.get(out_data, 2);
    buffer.put(data, data.size());
    EXPECT_EQ(buffer.size(), 4);

    buffer.get(out_data, 4);
    EXPECT_EQ(out_data[0], 3);
    EXPECT_EQ(out_data[1], 1);
    EXPECT_EQ(out_data[2], 2);
    EXPECT_EQ(out_data[3], 3);
}
