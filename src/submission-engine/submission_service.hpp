#pragma once
#include <grpcpp/grpcpp.h>
#include "submission.grpc.pb.h"
#include "sandbox.hpp"

namespace iicpc::submission {

class SubmissionServiceImpl final : public SubmissionService::Service {
public:
    grpc::Status Submit(grpc::ServerContext* context, const SubmissionRequest* request, SubmissionResponse* response) override;
    grpc::Status Deploy(grpc::ServerContext* context, const DeployRequest* request, DeployResponse* response) override;
private:
    SandboxManager sandbox_manager_;
};

}

