CXX = g++
CXXFLAGS = -std=c++20 -O3 -march=native -Wall -pthread
LDFLAGS = -lgrpc++ -lprotobuf -lhiredis -lpqxx -lpq

PROTO_DIR = src/shared/proto
PROTO_SRCS = $(PROTO_DIR)/submission.pb.cc $(PROTO_DIR)/submission.grpc.pb.cc \
             $(PROTO_DIR)/bot_fleet.pb.cc $(PROTO_DIR)/bot_fleet.grpc.pb.cc \
             $(PROTO_DIR)/telemetry.pb.cc $(PROTO_DIR)/telemetry.grpc.pb.cc

all: submission bot-fleet telemetry

protos:
	protoc -I=$(PROTO_DIR) --cpp_out=$(PROTO_DIR) --grpc_out=$(PROTO_DIR) \
	--plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $(PROTO_DIR)/*.proto

submission: protos src/submission-engine/*.cpp
	mkdir -p bin
	$(CXX) $(CXXFLAGS) src/submission-engine/*.cpp $(PROTO_SRCS) -o bin/$@ $(LDFLAGS)

bot-fleet: protos src/bot-fleet/*.cpp
	mkdir -p bin
	$(CXX) $(CXXFLAGS) src/bot-fleet/*.cpp $(PROTO_SRCS) -o bin/$@ $(LDFLAGS)

telemetry: protos src/telemetry-ingester/*.cpp
	mkdir -p bin
	$(CXX) $(CXXFLAGS) src/telemetry-ingester/*.cpp $(PROTO_SRCS) -o bin/$@ $(LDFLAGS)

clean:
	rm -rf bin/* $(PROTO_DIR)/*.pb.cc $(PROTO_DIR)/*.pb.h

