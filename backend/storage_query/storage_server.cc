#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>

#include <grpc++/grpc++.h>

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

class StorageServiceImpl final : public StorageQuery::Service{
	Status Get(ServerContext* context, const GetRequest* request, 
						GetResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();

		// notify if not found
		std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
					::const_iterator rfind = map.find(row);
		if (rfind == map.end()) {
			Status status(StatusCode::NOT_FOUND, "Row does not exist");
			return status;
		}
		std::unordered_map<std::string,std::string>::const_iterator cfind = map.at(row).find(col);
		if (cfind == map.at(row).end()) {
			Status status(StatusCode::NOT_FOUND, "Column does not exist");
			return status;
		}

		std::string val = map.at(row).at(col);
		response->set_val(val);
		return Status::OK;
	}

	Status Put(ServerContext* context, const PutRequest* request, 
						PutResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();
		std::string val = request->val();
		map[row][col] = val;

		return Status::OK;
	}

	Status CPut(ServerContext* context, const CPutRequest* request, 
						CPutResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();
		std::string val1 = request->val1();
		std::string val2 = request->val2();
		if (map[row][col] == val1) {
			map[row][col] = val2;
		}

		return Status::OK;
	}

	Status Delete(ServerContext* context, const DeleteRequest* request, 
						DeleteResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();
		map[row].erase(col);

		return Status::OK;
	}

private:
	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
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

  	return 0;
}