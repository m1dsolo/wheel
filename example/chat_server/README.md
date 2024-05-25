## Introduction

A simple chat server based on `wheel`.

features:
1. group chat

## Install

```bash
cmake -B build -DBUILD_CHAT_SERVER=1
cmake --build build -j4
```

## Usage

1. run server

```bash
./build/example/chat_server/chat_server
```
You can modify the `port` and `num_threads` of ThreadPool in `chat_server.json`.

2. run client

Use telnet to connect to the server:
```bash
telnet 127.0.0.1 12345
```
Or use simple tui client written in python:
```bash
python example/client/client.py 127.0.0.1 12345
```
Then input `name=xxx` to set your name and `group=xxx` to set your group.
The server will broadcast your message to the group you set.

