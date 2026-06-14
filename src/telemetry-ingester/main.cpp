#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "telemetry_service.hpp"

int main() {
    std::string server_address("0.0.0.0:50053");
    iicpc::telemetry::TelemetryServiceImpl service;
    service.initialize();

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Telemetry Ingester listening on " << server_address << std::endl;
    server->Wait();
    return 0;
}

