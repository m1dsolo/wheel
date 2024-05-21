## Introduction

A simple echo server based on `wheel`.

## Install

```bash
cmake -B build -DBUILD_ECHO_SERVER=1
cmake --build build -j4
```

## Usage

1. run server

```bash
./build/example/echo_server/echo_server
```

2. run client
```bash
telnet 127.0.0.1 12345
```
You can modify the `port` and `num_threads` of ThreadPool in `echo_server.json`.
