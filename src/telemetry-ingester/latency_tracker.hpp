#pragma once
#include <array>
#include <atomic>
#include <cstdint>

namespace iicpc::telemetry {

constexpr size_t MAX_BUCKETS = 10000; // Track up to 10ms in 1us increments

class LatencyTracker {
public:
    LatencyTracker() { clear(); }

    void record(double latency_ms) {
        size_t idx = static_cast<size_t>(latency_ms * 1000);
        if (idx >= MAX_BUCKETS) idx = MAX_BUCKETS - 1;
        buckets_[idx].fetch_add(1, std::memory_order_relaxed);
        total_.fetch_add(1, std::memory_order_relaxed);
    }

    double p99() const {
        uint64_t target = total_.load(std::memory_order_relaxed) * 0.99;
        uint64_t current = 0;
        for (size_t i = 0; i < MAX_BUCKETS; ++i) {
            current += buckets_[i].load(std::memory_order_relaxed);
            if (current >= target) return i / 1000.0;
        }
        return MAX_BUCKETS / 1000.0;
    }

    void clear() {
        for(auto& b : buckets_) b.store(0, std::memory_order_relaxed);
        total_.store(0, std::memory_order_relaxed);
    }
private:
    std::array<std::atomic<uint64_t>, MAX_BUCKETS> buckets_;
    std::atomic<uint64_t> total_{0};
};

}

