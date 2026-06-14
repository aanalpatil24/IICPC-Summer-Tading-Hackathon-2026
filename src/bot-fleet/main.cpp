#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "bot_fleet_service.hpp"

int main() {
    std::string server_address("0.0.0.0:50052");
    iicpc::botfleet::BotFleetServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Bot Fleet Load Generator listening on " << server_address << std::endl;
    server->Wait();
    return 0;
}

