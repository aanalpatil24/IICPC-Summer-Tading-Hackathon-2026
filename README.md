# Distributed Benchmarking and Hosting Platform

A high-throughput, native-isolation distributed benchmarking platform engineered for the IICPC Summer Hackathon. Designed by a graduating systems engineering student, this platform rigorously evaluates competitive quantitative trading algorithms under simulated peak market volatility.

[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![Node.js](https://img.shields.io/badge/Node.js-18.x-green.svg)](https://nodejs.org/)
[![Redis](https://img.shields.io/badge/Redis-7.x-red.svg)](https://redis.io/)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-15-336791.svg)](https://www.postgresql.org/)

## Overview

Evaluating financial exchange simulations requires strict deterministic latency, massive concurrency handling, and absolute process security.

Instead of relying on heavy enterprise cloud abstractions or commercial container daemons, this platform is built on native Linux mechanics and open-source tooling. By implementing lock-free data structures in C++20 and isolating executions via native POSIX namespaces, the system achieves sub-millisecond overhead, enabling it to saturate and measure endpoints at **100,000+ Transactions Per Second (TPS)**.

---

## Architecture

The platform operates as a decoupled, four-part microservices distributed system.

1. **Submission & Sandboxing Engine (C++20):**
   - Manages code ingestion and native compilation using `g++ -O3 -march=native`.
   - Bypasses external container APIs to jail contestant binaries using raw Linux `unshare(CLONE_NEWPID | CLONE_NEWNS)` and `cgroups v2` for zero-overhead, kernel-level isolation.
2. **Bot Fleet (C++20):**
   - A distributed load generation engine.
   - Utilizes dedicated `std::thread` workers and nanosecond-precision sleeps to bombard the sandboxed endpoint, completely eliminating thread-pool scheduling overhead.

3. **Telemetry Ingester (C++20):**
   - An asynchronous data pipeline calculating real-time percentiles (p50, p90, p99).
   - Utilizes an `std::array<std::atomic<uint64_t>, 10000>` to represent latency buckets. This lock-free architecture allows $O(1)$ updates, effortlessly processing over 100k TPS without mutex contention.

4. **Leaderboard Service (Node.js):**
   - A decoupled WebSocket engine.
   - Handles the I/O-bound task of broadcasting state changes and rankings to the frontend UI at 1Hz, offloading connection management from the execution engines.

---

## Inter-Service Communication

To optimize for maximum throughput and minimal memory footprint, the platform separates control and data planes:

- **Control Plane (gRPC):** Utilized for strict, typed communication across microservices (e.g., triggering compilation or spawning bots).
- **Data Plane (Redis Streams):** Chosen as a lightweight, memory-native alternative to heavy message brokers. The Bot Fleet pushes latency marks via the `hiredis` C-library, and the Ingester pulls via a blocking `XREAD` loop.

---

## Quick Start / Deployment

The platform is orchestrated entirely via open-source tools to ensure environment parity across local development and production.

> **Note:** The platform requires a Linux host with `cgroups v2` enabled. The `submission-engine` container runs in privileged mode to allow native kernel namespace manipulation for sandboxing.

### Prerequisites

- Docker & Docker Compose
- Linux environment (or WSL2)

### Bootstrapping the Cluster

```bash
# 1. Clone the repository
git clone [ https://github.com/aanalpatil24/IICPC-Summer-Tading-Hackathon-2026.git](https://github.com/aanalpatil24/IICPC-Summer-Tading-Hackathon-2026.git)
# 2. Start the backing infrastructure (Redis & PostgreSQL)
docker-compose up -d redis postgres

# 3. Build and spin up the execution pipeline
docker-compose up -d --build submission-engine bot-fleet telemetry-ingester

# 4. Start the Web UI
docker-compose up -d --build leaderboard

# 5. Verify services are running
docker-compose ps
```
