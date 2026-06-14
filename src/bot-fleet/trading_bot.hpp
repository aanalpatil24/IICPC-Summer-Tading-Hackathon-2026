#pragma once
#include <string>
#include <atomic>
#include "bot_fleet.pb.h"

namespace iicpc::botfleet {

class TradingBot {
public:
    TradingBot(const BotConfig& config) : config_(config), running_(true) {}
    void runWorker(const std::string& target_url, const std::string& bench_id);
    void stop() { running_ = false; }
private:
    BotConfig config_;
    std::atomic<bool> running_;
};

}

