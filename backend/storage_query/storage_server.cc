#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <sstream>
#include <signal.h>
#include <fcntl.h>

#include <grpc++/grpc++.h>

#include "storage_query.grpc.pb.h"

#include "cache.h"
#include "storage_client.h"
#include "master_client.h"

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
using storagequery::MigrateRequest;
using storagequery::MigrateResponse;
using storagequery::PingRequest;
using storagequery::PingResponse;

#define MASTER_ADDR "0.0.0.0:8000"
#define WORKER_ADDR "0.0.0.0:50051"

Logger wLogger;

class StorageServiceImpl final : public StorageQuery::Service{
	// TODO: add method to migrate date
	// Status Migrate(context, request, response)
	// request: address (hash(address) -> long)
	// response: string serialized map
	
	Status Get(ServerContext* context, const GetRequest* request, 
						GetResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();

		// notify if not found
		// std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
		// 			::const_iterator rfind = map.find(row);
		// if (rfind == map.end()) {
		// if(!cache.containsKey(row, col)) {
		// 	Status status(StatusCode::NOT_FOUND, "Row does not exist");
		// 	return status;
		// }
		// std::unordered_map<std::string,std::string>::const_iterator cfind = map.at(row).find(col);
		// if (cfind == map.at(row).end()) {
		// 	Status status(StatusCode::NOT_FOUND, "Column does not exist");
		// 	return status;
		// }

		// std::string val = map.at(row).at(col);

		try {
			std::string val = cache.get(row, col);
			response->set_val(val);
			return Status::OK;
		} catch (std::exception &e) {
			Status status(StatusCode::NOT_FOUND, "No corresponding keys");
			return status;
		}

		// std::string val = cache.get(row, col);
		// response->set_val(val);
		// return Status::OK;
		
	}

	Status Put(ServerContext* context, const PutRequest* request, 
						PutResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();
		std::string val = request->val();
		// map[row][col] = val;
		bool status = cache.put(row, col, val);

		// if(!status) {
		// 	Status status(StatusCode::ERROR, "ERROR");
		// 	return status;
		// }

		return Status::OK;
	}

	Status CPut(ServerContext* context, const CPutRequest* request, 
						CPutResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();
		std::string val1 = request->val1();
		std::string val2 = request->val2();
		// if (map[row][col] == val1) {
		// 	map[row][col] = val2;
		// }

		cache.cput(row, col, val1, val2);

		return Status::OK;
	}

	Status Delete(ServerContext* context, const DeleteRequest* request, 
						DeleteResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();
		// map[row].erase(col);

		cache.remove(row, col);

		return Status::OK;
	}

	Status Migrate(ServerContext* context, const MigrateRequest* request,
						MigrateResponse* response) override {
		// TODO: implement
		std::string address = request->address();
		response->set_data("123");
		return Status::OK;
	}

	Status Ping(ServerContext* context, const PingRequest* request,
						PingResponse* response) override {
		return Status::OK;
	}

public:
	// std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;
	Cache cache;

};

std::string get_real_addr(std::string virtual_addr) {

}

void get_data() {
	MasterClient master(grpc::CreateChannel(MASTER_ADDR, grpc::InsecureChannelCredentials()));
	if (master.Ping()) {
		wLogger.log_trace("master is ready, requesting data");
		std::vector<std::pair<std::string, std::string> > pairs;
		if (master.Get(WORKER_ADDR, NULL, pairs)) {
			for (std::pair<std::string, std::string> p : pairs) {
				std::string other = get_real_addr(p.first);
				StorageClient worker(grpc::CreateChannel(other, grpc::InsecureChannelCredentials()));
				StorageClient self(grpc::CreateChannel(WORKER_ADDR, grpc::InsecureChannelCredentials()));
				std::unordered_map<std::string, std::unordered_map<std::string, std::string> > data;
				worker.Migrate(p.second, data);
				for (auto it = data.begin(); it != data.end(); it++) {
					for (auto itr = it->second.begin(); itr != it->second.end(); itr++) {
						self.Put(it->first, itr->first, itr->second);
					}
				}
			}
		}

	} else {
		wLogger.log_error("master is down");
	}
}


void RunServer() {

	std::string server_address(WORKER_ADDR);
	StorageServiceImpl service;

	ServerBuilder builder;

	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service);
	// Finally assemble the server.
	std::unique_ptr<Server> server(builder.BuildAndStart());
	wLogger.log_trace("Server listening on " + server_address);

	// pthread_create(&thread, NULL, worker, NULL);

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}



int main(int argc, char** argv) {
	wLogger.log_config("StorageServer");

	get_data();
	
	RunServer();

	std::cout << "test" << std::endl;

  	return 0;
}
