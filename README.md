## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Install](#install)
- [License](#license)

## Introduction

A C++ library for various types of wheels(Based on c++23).
This library is mainly written for learning purposes.
The code structure is clear and easy to read.

## Features

1. ThreadPool: thread pool.
2. Json: json parser.
3. Enum: Conversion between `enum` and `string` based on reflection.
4. Log: for logging and assert.
5. Socket: Encapsulation of `c` socket api.
6. Epoll: Encapsulation of `c` epoll api.
7. Server: Abstract server in reactor mode. (See [chat](https://github.com/m1dsolo/chat.git) for more info.)
8. Singleton: Singleton base class.
9. Csv: Read and parse csv file.
10. Utils: Some useful functions.
11. Random: Random number generator.
12. Timer: Timer based on `std::chrono`.
13. ObjectPool: Object pool.
14. geometry: Geometry calculation.
15. QuadTree: Quad tree.
16. RingBuffer: Ring buffer.
17. ID: faster than string

For usage examples, please refer to the test cases in the `test` directory.
I will update `wiki` in the future.

## Install

Feel free to copy code into your project.

### Use `wheel` as a submodule

1. Add `wheel` to your project

```bash
git submodule add --depth=1 https://github.com/m1dsolo/wheel.git third_party/wheel
```

2. Modify your `CMakeLists.txt` file.

```cmake
add_subdirectory(third_party/wheel)
target_link_libraries(${YOUR_TARGET} third_party/wheel)
target_include_directories(${YOUR_TARGET} PRIVATE third_party/wheel/include)
```

### Test `wheel` with `googletest`

```bash
git clone https://github.com/m1dsolo/wheel.git
cd wheel
git submodule update --init --depth=1
cmake -B build -DBUILD_WHEEL_TEST=1
cmake --build build -j4
./build/test/wheel_test  # run test
```

## License

[MIT](LICENSE) © m1dsolo
