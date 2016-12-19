#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <unistd.h>
#include "ConHash.h"
#include "logger.h"
#include "pthread.h"
#include <grpc++/grpc++.h>

#include "storage_client.h"

#include "storage_query.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;
using storagequery::StorageQuery;
using storagequery::GetNodeRequest;
using storagequery::GetNodeResponse;
using storagequery::AddNodeRequest;
using storagequery::AddNodeResponse;
using storagequery::GetReplicaRequest;
using storagequery::GetReplicaResponse;
using storagequery::PingRequest;
using storagequery::PingResponse;
using storagequery::GetAllNodesRequest;
using storagequery::GetAllNodesResponse;

#define SERVER_CONFIG "./../config/servers.config"

Logger mLogger;
ConHash conHash;
std::vector<std::string> servers; //for testing

class StorageServiceImpl final : public StorageQuery::Service{

	int hash(std::string str) {
		return str.length() - 1;
	}

	Status GetNode(ServerContext* context, const GetNodeRequest* request, 
						GetNodeResponse* response) override {

		std::string key = request->row();

		try {
			std::vector<std::string> nodes = conHash.getNodes(key);	// get nodes that stores the key
			std::string res;
			for(std::string node: nodes) {
				res.append(node).append(" ");
			}
			response->set_addr(res);
			return Status::OK;
		} catch (std::exception &e) {
			Status status(StatusCode::NOT_FOUND, "No corresponding keys");
			return status;
		}
		
	}

	Status GetReplica(ServerContext* context, const GetReplicaRequest* request, 
						GetReplicaResponse* response) override {

		std::string key = request->row();

		std::string replica = conHash.getReplica(key);
		response->set_addr(replica);		

		return Status::OK;
	}

	Status AddNode(ServerContext* context, const AddNodeRequest* request, 
						AddNodeResponse* response) override {
		std::string addr = request->addr();
		try {
			std::vector<std::pair<std::string, std::string> > pairs = conHash.addNode(addr);

			std::string res;

			for(int i = 0; i < pairs.size(); i++) {
				std::pair<std::string, std::string> p = pairs[i];

				res.append(p.first).append(" ").append(p.second).append(",");
			}

			response->set_data_map(res);
		} catch (std::exception& e) { }

		mLogger.log_trace("Node " + addr + " added successfully.");
		return Status::OK;
	}

	Status Ping(ServerContext* context, const PingRequest* request,
						PingResponse* response) override {
		return Status::OK;
	}

	Status GetAllNodes(ServerContext* context, const GetAllNodesRequest* request,
						GetAllNodesResponse* response) override {
		std::string up = conHash.getUpServers();
		std::string down = conHash.getDownServers();
		response->set_up(up);
		response->set_down(down);
		return Status::OK;
	}
};

void* check_servers(void*) {
	while (true) {
		std::vector<std::string> servers = conHash.getAllNodes();
		for (std::string server : servers) {
			mLogger.log_trace("Checking " + server);
			StorageClient client(grpc::CreateChannel(server, grpc::InsecureChannelCredentials()));
			if (client.Ping()) {
				mLogger.log_trace("Server " + server + " OK.");
				conHash.notifyUp(server);
			} else {
				mLogger.log_warn("Server " + server + " is down.");
				conHash.notifyDown(server);
			}
		}
		sleep(2);
	}
}

void RunServer() {

	std::string server_address("0.0.0.0:8000");
	StorageServiceImpl service;

	ServerBuilder builder;
	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service);
	// Finally assemble the server.
	std::unique_ptr<Server> server(builder.BuildAndStart());
	mLogger.log_trace("Server listening on " + server_address);

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}


int main(int argc, char** argv) {
	mLogger.log_config("Master");


	pthread_t ping_thread;
	if (0 != pthread_create(&ping_thread, NULL, &check_servers, NULL)) {
		mLogger.log_error("ping thread create failure");
	}

	RunServer();

	// ConHash conHash;

	// conHash.addNode("localhost:50051");

	// conHash.addNode("localhost:50052");

	// std::string row; 
	// std::string col; 
	// std::string val; 

	// std::vector<std::string> vec;

	// for(int i = 0; i < 100; i++) {
	// 	row = "r" + std::to_string(i);
	// 	col = "c" + std::to_string(i);
	// 	// val = "v" + std::to_string(i);
	// 	vec = conHash.getNodes(row);

	// 	std::cout << "(" << row << ", " << col << ") is in node: " << vec[0] << std::endl;
	// }

  	return 0;
}
