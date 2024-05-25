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
You can modify the `port` and `num_threads` of ThreadPool in `echo_server.json`.

2. run client

Use telnet to connect to the server:
```bash
telnet 127.0.0.1 12345
```
Or use simple tui client written in python:
```bash
python example/client/client.py 127.0.0.1 12345
```
