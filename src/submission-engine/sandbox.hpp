#pragma once
#include <string>
#include <sys/types.h>

namespace iicpc::submission {

struct SandboxConfig {
    std::string submission_id;
    int cpu_cores = 2;
    int memory_mb = 512;
};

class SandboxManager {
public:
    SandboxManager();
    bool initialize();
    pid_t createSandbox(const SandboxConfig& config, const std::string& binary_path);
};

}

