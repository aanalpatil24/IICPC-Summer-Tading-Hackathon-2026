#pragma once
#include <grpcpp/grpcpp.h>
#include <thread>
#include <atomic>
#include "telemetry.grpc.pb.h"
#include "latency_tracker.hpp"

namespace iicpc::telemetry {

class TelemetryServiceImpl final : public TelemetryService::Service {
public:
    bool initialize();
    ~TelemetryServiceImpl() { running_ = false; if (consumer_thread_.joinable()) consumer_thread_.join(); }
    grpc::Status GetAggregateScore(grpc::ServerContext* context, const QueryRequest* request, AggregateScore* response) override;
private:
    void consumeRedisStream();
    std::thread consumer_thread_;
    std::atomic<bool> running_{true};
    LatencyTracker tracker_;
};

}

