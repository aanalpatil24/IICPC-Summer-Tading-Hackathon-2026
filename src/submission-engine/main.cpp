#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "submission_service.hpp"

int main() {
    std::string server_address("0.0.0.0:50051");
    iicpc::submission::SubmissionServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Submission Engine (Native Isolation) listening on " << server_address << std::endl;
    server->Wait();
    return 0;
}

