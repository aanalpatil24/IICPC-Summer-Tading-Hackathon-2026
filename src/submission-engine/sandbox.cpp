#include "sandbox.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/mount.h>
#include <sys/resource.h>

namespace iicpc::submission {
namespace fs = std::filesystem;

SandboxManager::SandboxManager() {}

bool SandboxManager::initialize() {
    fs::create_directories("/var/lib/iicpc/sandboxes");
    fs::create_directories("/sys/fs/cgroup/iicpc");
    return true;
}

pid_t SandboxManager::createSandbox(const SandboxConfig& config, const std::string& binary_path) {
    std::string container_id = "iicpc-" + config.submission_id;
    std::string sandbox_path = "/var/lib/iicpc/sandboxes/" + container_id;
    fs::create_directories(sandbox_path + "/rootfs");
    fs::copy(binary_path, sandbox_path + "/rootfs/engine", fs::copy_options::overwrite_existing);
    fs::permissions(sandbox_path + "/rootfs/engine", fs::perms::owner_exec);

    std::string cgroup_path = "/sys/fs/cgroup/iicpc/" + container_id;
    fs::create_directories(cgroup_path);
    std::ofstream(cgroup_path + "/cpu.max") << (config.cpu_cores * 100000) << " 100000";
    std::ofstream(cgroup_path + "/memory.max") << (config.memory_mb * 1024 * 1024);

    pid_t pid = fork();
    if (pid == 0) {
        std::ofstream(cgroup_path + "/cgroup.procs") << getpid();
        unshare(CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC);
        chroot(sandbox_path.c_str());
        chdir("/");
        setgid(65534);
        setuid(65534);
        execlp("/rootfs/engine", "/rootfs/engine", nullptr);
        _exit(127);
    }
    return pid;
}

}

