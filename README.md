## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Install](#install)
- [License](#license)

## Introduction

A C++ library for various types of wheels(Based on c++20).
This library is mainly written for learning purposes.
The code structure is clear and easy to read.

## Features

1. ThreadPool: thread pool.
2. Json: json parser.
3. Enum: Conversion between `enum` and `string` based on reflection.
4. Log: for logging and assert.
5. Socket: Encapsulation of `c` socket api.
6. Epoll: Encapsulation of `c` epoll api.
7. Server: Abstract server in reactor mode.

    - [example/echo_server](./example/echo_server): A simple echo server.
    - [example/chat_server](./example/chat_server): A simple chat server.

        [chat_intro_video](https://github.com/m1dsolo/wheel/assets/74849775/664feba9-5724-4e6b-b46f-a330101b21fa)

8. Singleton: Singleton base class.
9. Csv: Read and parse csv file.
10. Utils: Some useful functions.
11. Random: Random number generator.
12. Timer: Timer based on `std::chrono`.
13. ObjectPool: Object pool.
14. ECS: Entity-Component-System.
15. geometry: Geometry calculation.
16. QuadTree: Quad tree.

For usage examples, please refer to the test cases in the `test` directory.
I will update `wiki` in the future.

## Install

Feel free to copy code into your project.

### Use as `wheel` as a static library

Clone the repository and add it to your project.
```bash
git clone https://github.com/m1dsolo/wheel.git
```

Modify your `CMakeLists.txt` file.
```cmake
add_subdirectory(wheel)
target_link_libraries(${YOUR_TARGET} wheel)
target_include_directories(${YOUR_TARGET} PRIVATE wheel/include)
```

### Test `wheel` with `googletest`

```bash
git clone --recursive https://github.com/m1dsolo/wheel.git  # `--recursive` for googletest
cd path/to/project
cmake -B build -DBUILD_WHEEL_TEST=1
cmake --build build -j4
# add flag `-DCMAKE_BUILD_TYPE=Debug` to use debug mode instead of release mode
./build/test/wheel_test  # run test
```

## License

[MIT](LICENSE) © m1dsolo

