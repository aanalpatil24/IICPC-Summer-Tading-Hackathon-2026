CREATE TABLE submissions (
    submission_id TEXT PRIMARY KEY,
    team_id TEXT NOT NULL,
    status TEXT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    endpoint_url TEXT
);

CREATE TABLE telemetry_aggregates (
    time_bucket TIMESTAMPTZ NOT NULL,
    benchmark_id TEXT NOT NULL,
    p50_latency_ms DOUBLE PRECISION,
    p99_latency_ms DOUBLE PRECISION,
    transactions_per_sec BIGINT,
    PRIMARY KEY (time_bucket, benchmark_id)
);

CREATE TABLE aggregate_scores (
    submission_id TEXT PRIMARY KEY REFERENCES submissions(submission_id),
    benchmark_id TEXT NOT NULL,
    composite_score DOUBLE PRECISION NOT NULL,
    latency_score DOUBLE PRECISION NOT NULL,
    throughput_score DOUBLE PRECISION NOT NULL,
    computed_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE INDEX idx_scores_composite ON aggregate_scores (composite_score DESC);

