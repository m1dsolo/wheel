#include <chat_handler.hpp>

#include <format>

#include <wheel/log.hpp>

namespace wheel {

void ChatManager::set_group(std::shared_ptr<Socket> socket, int group) {
    if (socket2groups_.count(socket)) {
        del(socket);
    }
    socket2groups_[socket] = group;
    groups_[group].insert(socket);
}

void ChatManager::set_name(std::shared_ptr<Socket> socket, const std::string &name) {
    socket2names_[socket] = name;
}

void ChatManager::del(std::shared_ptr<Socket> socket) {
    int group = socket2groups_[socket];
    socket2names_.erase(socket);
    socket2groups_.erase(socket);
    groups_[group].erase(socket);
}

const std::optional<std::string_view> ChatManager::get_name(std::shared_ptr<Socket> socket) {
    if (!socket2names_.count(socket)) {
        return std::nullopt;
    }
    return socket2names_[socket];
}

const std::optional<std::reference_wrapper<std::unordered_set<std::shared_ptr<Socket>>>> ChatManager::get_group(std::shared_ptr<Socket> socket) {
    if (!socket2groups_.count(socket)) {
        return std::nullopt;
    }
    return std::ref(groups_[socket2groups_[socket]]);
}

ChatHandler::~ChatHandler() {
    chat_manager_.del(socket_);
}

bool ChatHandler::process() {
    if (!recv()) [[unlikely]] return false;

    std::string_view s(buf_);
    if (s == "") {
        Log::info("close({}:{})", socket_->get_peer_ip(), socket_->get_peer_port());
        return false;
    }
    // remove '\n'
    if (s.back() == '\n') {
        s.remove_suffix(1);
    }

    // set name
    if (s.substr(0, 5) == "name=") {
        chat_manager_.set_name(socket_, std::string(s.substr(5)));

        if (!send(socket_, std::format("your name is: {} now!\n", s.substr(5)))) [[unlikely]] return false;
    // set group
    } else if (s.substr(0, 6) == "group=") {
        int group;
        try {
            group = std::stoi(std::string(s.substr(6)));
        } catch (const std::invalid_argument& e) {
            if (!send(socket_, "invalid argument\n")) [[unlikely]] return false;
        } catch (const std::out_of_range& e) {
            if (!send(socket_, "out of range, please input\n")) [[unlikely]] return false;
        }
        chat_manager_.set_group(socket_, group);

        std::string_view name = chat_manager_.get_name(socket_).value_or("unknown");
        if (!send(socket_, std::format("you are in group: {} now!\n", group))) [[unlikely]] return false;
        if (!broadcast(std::format("Welcome {} join group {}!\n", name, group), false)) [[unlikely]] return false;
    // broadcast message
    } else {
        std::string_view name = chat_manager_.get_name(socket_).value_or("unknown");
        if (!broadcast(std::format("{}: {}\n", name, s))) [[unlikely]] return false;
    }

    return true;
}

bool ChatHandler::recv() {
    if (!socket_->recv(buf_)) {
        Log::error("socket recv error");
        return false;
    }
    Log::info("recv({}:{}): {}", socket_->get_peer_ip(), socket_->get_peer_port(), buf_);
    return true;
}

bool ChatHandler::send(std::shared_ptr<Socket> socket, std::string_view msg) {
    if (!socket->send(msg)) {
        Log::error("socket send error");
        return false;
    }
    if (msg.back() == '\n') {
        msg.remove_suffix(1);
    }
    Log::info("send({}:{}): {}", socket->get_peer_ip(), socket->get_peer_port(), msg);
    return true;
}

bool ChatHandler::broadcast(std::string_view msg, bool include_self) {
    auto group = chat_manager_.get_group(socket_);
    if (group) {
        for (auto target : (*group).get()) {
            if (!include_self && target == socket_) {
                continue;
            }
            if (!send(target, msg)) [[unlikely]] return false;
        }
    } else {
        if (!send(socket_, msg)) [[unlikely]] return false;
        if (!send(socket_, "(you are not in one group!)\n")) [[unlikely]] return false;
    }

    return true;
}

wheel::ChatManager wheel::ChatHandler::chat_manager_;

}  // namespace wheel

