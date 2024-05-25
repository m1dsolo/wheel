#include <filesystem>

#include <wheel/json.hpp>
#include <wheel/server.hpp>
#include <wheel/utils.hpp>

#include <chat_handler.hpp>

int main(int argc, const char *argv[]) {
    int port = 12345;
    int num_threads = 4;
    wheel::JsonObject config = wheel::Json::parse_file(std::filesystem::path(argv[0]).parent_path() / "chat_server.json");
    if (!config.empty()) {
        port = int(config["port"]);
        num_threads = int(config["num_threads"]);
        wheel::Log::info("Loaded chat_server.json(port={}, num_threads={}).", port, num_threads);
    } else {
        wheel::Log::info("Failed to load chat_server.json, using default values(port=12345, num_threads=4).");
    }

    auto server = wheel::Server();
    server.start(
        []() {return std::make_shared<wheel::ChatHandler>();},
        port,
        num_threads
    );

    return 0;
}
