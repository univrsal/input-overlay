#pragma once
#include <mutex>
#include <string>

// Separate mongoose interface as it seems to clash with libgamepad headers
// probably some directinput stuff
namespace mg {
extern std::mutex poll_mutex;
bool start(const std::string &addr);
void stop();
void queue_message(const std::string &msg);
bool can_queue_message();
}