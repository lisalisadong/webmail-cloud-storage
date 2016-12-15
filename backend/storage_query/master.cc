#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>
#include "ConHash.h"
#include "logger.h"
#include <grpc++/grpc++.h>

#include "storage_client.h"

#include "storage_query.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;
using storagequery::StorageQuery;
using storagequery::GetRequest;
using storagequery::GetResponse;
using storagequery::PutRequest;
using storagequery::PutResponse;
using storagequery::CPutRequest;
using storagequery::CPutResponse;
using storagequery::DeleteRequest;
using storagequery::DeleteResponse;

ConHash conHash;

class StorageServiceImpl final : public StorageQuery::Service{

	int hash(std::string str) {
		return str.length() - 1;
	}

	Status Get(ServerContext* context, const GetRequest* request, 
						GetResponse* response) override {

		std::string row = request->row();

		try {
			response->set_val("");
			return Status::OK;
		} catch (std::exception &e) {
			Status status(StatusCode::NOT_FOUND, "No corresponding keys");
			return status;
		}
		
	}

	Status Put(ServerContext* context, const PutRequest* request, 
						PutResponse* response) override {

		return Status::OK;
	}

	Status CPut(ServerContext* context, const CPutRequest* request, 
						CPutResponse* response) override {

		return Status::OK;
	}

	Status Delete(ServerContext* context, const DeleteRequest* request, 
						DeleteResponse* response) override {

		return Status::OK;
	}

	Status Migrate(ServerContext* context, const MigrateRequest* request,
						MigrateResponse* response) override {
		std::string address = request->address();
		response->set_data("123");
		return Status::OK;
	}


};


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
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}


int main(int argc, char** argv) {
	RunServer();
	ConHash conHash;

	conHash.addNode("127.0.0.1:8000");

	conHash.getReplicaNode("127.0.0.1:8000");

	conHash.addNode("127.0.0.1:8001");

	conHash.addNode("127.0.0.1:8002");

	conHash.addNode("127.0.0.1:8003");

	conHash.addNode("127.0.0.1:8004");

	conHash.addNode("127.0.0.1:8005");

	conHash.getReplicaNode("127.0.0.1:8000");

  	return 0;
}
