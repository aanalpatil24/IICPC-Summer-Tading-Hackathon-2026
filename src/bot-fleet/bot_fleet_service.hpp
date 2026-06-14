#pragma once
#include <grpcpp/grpcpp.h>
#include <vector>
#include <memory>
#include <thread>
#include "bot_fleet.grpc.pb.h"
#include "trading_bot.hpp"

namespace iicpc::botfleet {

class BotFleetServiceImpl final : public BotFleetService::Service {
public:
    grpc::Status SpawnBots(grpc::ServerContext* context, const SpawnRequest* request, SpawnResponse* response) override;
private:
    std::vector<std::shared_ptr<TradingBot>> active_bots_;
    std::vector<std::thread> bot_threads_;
};

}

