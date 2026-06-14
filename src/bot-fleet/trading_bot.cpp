#include "trading_bot.hpp"
#include <chrono>
#include <thread>
#include <cstdlib>
#include <hiredis/hiredis.h>

namespace iicpc::botfleet {

void TradingBot::runWorker(const std::string& target_url, const std::string& bench_id) {
    const char* redis_host = std::getenv("REDIS_HOST") ? std::getenv("REDIS_HOST") : "127.0.0.1";
    redisContext* redis = redisConnect(redis_host, 6379);
    if (!redis || redis->err) return;

    int64_t target_interval_us = 1000000 / config_.orders_per_second();

    while (running_) {
        auto start = std::chrono::high_resolution_clock::now();

        // Target system execution (simulated HTTP I/O logic placed here)

        auto end = std::chrono::high_resolution_clock::now();
        double latency_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;

        // Zero-overhead IPC to Telemetry Ingester via Redis Streams
        redisCommand(redis, "XADD telemetry:%s * latency %f", bench_id.c_str(), latency_ms);

        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        if (elapsed_us < target_interval_us) {
            std::this_thread::sleep_for(std::chrono::microseconds(target_interval_us - elapsed_us));
        }
    }
    redisFree(redis);
}

}

