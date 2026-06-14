#include "bot_fleet_service.hpp"

namespace iicpc::botfleet {

grpc::Status BotFleetServiceImpl::SpawnBots(grpc::ServerContext* context, const SpawnRequest* request, SpawnResponse* response) {
    for (const auto& config : request->bots()) {
        auto bot = std::make_shared<TradingBot>(config);
        active_bots_.push_back(bot);
        for(int i = 0; i < config.concurrency(); i++) {
            bot_threads_.emplace_back(&TradingBot::runWorker, bot, config.endpoint_url(), request->benchmark_id());
        }
        response->add_bot_ids(config.bot_id());
    }
    response->set_benchmark_id(request->benchmark_id());
    response->set_success(true);
    return grpc::Status::OK;
}

}

