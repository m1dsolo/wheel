#pragma once

#include <wheel/socket_handler.hpp>
#include <wheel/socket.hpp>

#include <optional>
#include <unordered_set>
#include <unordered_map>

namespace wheel {

class ChatManager {
public:
    ChatManager() = default;
    ~ChatManager() = default;
    ChatManager(const ChatManager&) = delete;
    ChatManager& operator=(const ChatManager&) = delete;

    void set_group(std::shared_ptr<Socket> socket, int group);
    void set_name(std::shared_ptr<Socket> socket, const std::string &name);
    void del(std::shared_ptr<Socket> socket);
    const std::optional<std::string_view> get_name(std::shared_ptr<Socket> socket);
    const std::optional<std::reference_wrapper<std::unordered_set<std::shared_ptr<Socket>>>> get_group(std::shared_ptr<Socket> socket);

private:
    std::unordered_map<std::shared_ptr<Socket>, std::string> socket2names_;
    std::unordered_map<std::shared_ptr<Socket>, int> socket2groups_;
    std::unordered_map<int, std::unordered_set<std::shared_ptr<Socket>>> groups_;
};

class ChatHandler : public SocketHandler {
public:
    ChatHandler() = default;
    ~ChatHandler();
    ChatHandler(const ChatHandler &) = delete;
    ChatHandler& operator=(const ChatHandler&) = delete;

    virtual bool process() override; 

private:
    bool broadcast(std::string_view msg, bool include_self=true);
    bool recv();
    bool send(std::shared_ptr<Socket> socket, std::string_view msg);

    char buf_[1024];
    static ChatManager chat_manager_;
};

}  // namespace wheel

