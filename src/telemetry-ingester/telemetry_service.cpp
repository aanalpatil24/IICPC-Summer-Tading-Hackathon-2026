#include "telemetry_service.hpp"
#include <hiredis/hiredis.h>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace iicpc::telemetry {

bool TelemetryServiceImpl::initialize() {
    consumer_thread_ = std::thread(&TelemetryServiceImpl::consumeRedisStream, this);
    return true;
}

void TelemetryServiceImpl::consumeRedisStream() {
    const char* redis_host = std::getenv("REDIS_HOST") ? std::getenv("REDIS_HOST") : "127.0.0.1";
    redisContext* redis = redisConnect(redis_host, 6379);
    if (!redis || redis->err) return;

    std::string last_id = "$";

    while (running_) {
        // Epoll-backed blocking stream read
        redisReply* reply = (redisReply*)redisCommand(redis, "XREAD BLOCK 1000 STREAMS telemetry:bench_001 %s", last_id.c_str());

        if (reply != nullptr && reply->type == REDIS_REPLY_ARRAY) {
            tracker_.record(0.5); // Simulated parsed latency value

            // Logarithmic scoring curve
            double p99 = tracker_.p99();
            double score = std::max(0.0, 100.0 - (20.0 * std::log10(std::max(p99, 0.001))));

            // Push score to Leaderboard ZSET
            redisCommand(redis, "ZADD leaderboard %f bench_001", score);
        }
        freeReplyObject(reply);
    }
    redisFree(redis);
}

grpc::Status TelemetryServiceImpl::GetAggregateScore(grpc::ServerContext* context, const QueryRequest* request, AggregateScore* response) {
    response->set_composite_score(100.0 - (20.0 * std::log10(std::max(tracker_.p99(), 0.001))));
    return grpc::Status::OK;
}

}

