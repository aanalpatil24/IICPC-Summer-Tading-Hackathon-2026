#include "submission_service.hpp"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <unistd.h>
#include <sys/wait.h>

namespace iicpc::submission {

grpc::Status SubmissionServiceImpl::Submit(grpc::ServerContext* context, const SubmissionRequest* request, SubmissionResponse* response) {
    std::string sub_id = request->team_id() + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    std::string build_dir = "/tmp/iicpc/" + sub_id;
    std::filesystem::create_directories(build_dir);
    std::ofstream(build_dir + "/main.cpp") << request->source_code();

    pid_t pid = fork();
    if (pid == 0) {
        execlp("g++", "g++", "-std=c++20", "-O3", "-march=native",
               (build_dir + "/main.cpp").c_str(), "-o", (build_dir + "/engine").c_str(), nullptr);
        _exit(1);
    }

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        response->set_submission_id(sub_id);
        response->set_status("STATUS_SANDBOXED");
    } else {
        response->set_status("STATUS_FAILED");
    }
    return grpc::Status::OK;
}

grpc::Status SubmissionServiceImpl::Deploy(grpc::ServerContext* context, const DeployRequest* request, DeployResponse* response) {
    SandboxConfig config;
    config.submission_id = request->submission_id();

    sandbox_manager_.initialize();
    pid_t pid = sandbox_manager_.createSandbox(config, "/tmp/iicpc/" + config.submission_id + "/engine");

    response->set_submission_id(config.submission_id);
    response->set_container_id(std::to_string(pid));
    response->set_endpoint_url("http://127.0.0.1:8080/order");
    response->set_status("STATUS_DEPLOYED");
    return grpc::Status::OK;
}

}

