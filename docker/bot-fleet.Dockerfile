FROM gcc:13-bookworm
RUN apt-get update && apt-get install -y cmake libgrpc++-dev protobuf-compiler-grpc libhiredis-dev libpqxx-dev
WORKDIR /app
COPY . .
RUN make protos && make bot-fleet
CMD ["./bin/bot-fleet"]

