## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Install](#install)
- [License](#license)

## Introduction

A C++ library for various types of wheels.
This library is mainly written for learning purposes.
The code structure is clear and easy to read.

## Features

1. ThreadPool: A simple thread pool implementation using c++11 features.
2. Json: A simple json parser implementation.
3. Enum: Conversion between enum and string based on reflection.

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
cmake -B build
cmake --build build -j4 -DBUILD_WHEEL_TEST=1
# add flag `-DCMAKE_BUILD_TYPE=Debug` to use debug mode instead of release mode
./build/test/wheel_test  # run test
```

## License

[MIT](LICENSE) © m1dsolo

