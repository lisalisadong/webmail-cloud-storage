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
#include <signal.h>

#include <grpc++/grpc++.h>

#include "storage_query.grpc.pb.h"

#include "cache.h"
#include "storage_client.h"
#include "master_client.h"
#include "utils.h"

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
using storagequery::GetDataRequest;
using storagequery::GetDataResponse;
using storagequery::GetAllNodesRequest;
using storagequery::GetAllNodesResponse;

#define MASTER_ADDR "localhost:8000"

Logger wLogger;
std::string worker_addr("localhost:");
std::vector<std::pair<std::string, std::string> > putReplica;
std::vector<std::pair<std::string, std::string> > deleteReplica;

/* global master client */
MasterClient master(grpc::CreateChannel(MASTER_ADDR, grpc::InsecureChannelCredentials()));

class StorageServiceImpl final : public StorageQuery::Service{
	
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

		wLogger.log_trace("Getting (" + row + ", " + col + ")");

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

		wLogger.log_trace("Putting (" + row + ", " + col + "," + val + ")");

		// map[row][col] = val;
		bool status = cache.put(row, col, val);

		// not tested yet
		/* get replica addr from master */
		std::string replicaAddr;
		bool hasReplica = master.GetReplica(row, col, replicaAddr);

		wLogger.log_trace("replica: " + replicaAddr);

		if(hasReplica && replicaAddr != worker_addr) {
			wLogger.log_trace("putting copy to: " + replicaAddr);
			StorageClient replicaNode(grpc::CreateChannel(replicaAddr, grpc::InsecureChannelCredentials()));
			if (!replicaNode.Put(row, col, val)) {
				wLogger.log_trace("putting copy to cache " + row + col + val);
				putReplica.push_back(std::make_pair(row, col));
			}
		} else {
			wLogger.log_trace("copy was not put");
		}

		return Status::OK;
	}

	Status CPut(ServerContext* context, const CPutRequest* request, 
						CPutResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();
		std::string val1 = request->val1();
		std::string val2 = request->val2();

		wLogger.log_trace("CPutting (" + row + ", " + col + ")");

		// if (map[row][col] == val1) {
		// 	map[row][col] = val2;
		// }

		cache.cput(row, col, val1, val2);

		// not tested yet
		/* get replica addr from master */
		std::string replicaAddr;
		if(master.GetReplica(row, col, replicaAddr) && replicaAddr != worker_addr) {
			StorageClient replicaNode(grpc::CreateChannel(replicaAddr, grpc::InsecureChannelCredentials()));
			if (!replicaNode.CPut(row, col, val1, val2)) {
				wLogger.log_trace("putting copy to cache");
				putReplica.push_back(std::make_pair(row, col));
			}
		}

		return Status::OK;
	}

	Status Delete(ServerContext* context, const DeleteRequest* request, 
						DeleteResponse* response) override {

		std::string row = request->row();
		std::string col = request->col();

		wLogger.log_trace("Deleting (" + row + ", " + col + ")");

		// map[row].erase(col);

		cache.remove(row, col);

		// not tested yet
		/* get replica addr from master */
		std::string replicaAddr;
		if(master.GetReplica(row, col, replicaAddr) && replicaAddr != worker_addr) {
			StorageClient replicaNode(grpc::CreateChannel(replicaAddr, grpc::InsecureChannelCredentials()));
			if (!replicaNode.Delete(row, col)) {
				wLogger.log_trace("putting copy to cache");
				deleteReplica.push_back(std::make_pair(row, col));
			}
		}

		return Status::OK;
	}

	Status Migrate(ServerContext* context, const MigrateRequest* request,
						MigrateResponse* response) override {
		std::string address = request->addr();
		std::size_t found = address.find(" ");

		std::string self_addr = address.substr(0, found);

		std::string other_addr = address.substr(found + 1);

		std::string data;
		cache.migrate(self_addr, other_addr, data);

		response->set_data(data);
		return Status::OK;
	}

	Status Ping(ServerContext* context, const PingRequest* request,
						PingResponse* response) override {
		return Status::OK;
	}

	Status GetData(ServerContext* context, const GetDataRequest* request, 
						GetDataResponse* response) override {
		int start = request->start();
		int size = request->size();

		std::string data;
		int ret = cache.get_raw_data(start, size, data);
		wLogger.log_trace("get data size: " + std::to_string(ret));

		response->set_size(ret);
		response->set_data(data);

		return Status::OK;
	}

public:
	// std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;
	Cache cache = Cache::create_cache(worker_addr);
};

/* replay commands on replica if it was down */
void* replay_replica(void* ) {
	sleep(5);
	while (true) {
		sleep(1);
		while (putReplica.size() > 0) {
			std::pair<std::string, std::string> p = putReplica.back();
			putReplica.pop_back();
			std::string replica_addr;
			master.GetReplica(p.first, p.second, replica_addr);
			StorageClient primary(grpc::CreateChannel(worker_addr, grpc::InsecureChannelCredentials()));
			StorageClient replica(grpc::CreateChannel(replica_addr, grpc::InsecureChannelCredentials()));		
			if (primary.Ping() && replica.Ping()) {
				std::string val;
				std::cout << p.first + p.second + val << std::endl;
				primary.Get(p.first, p.second, val);
				replica.Put(p.first, p.second, val);
			} else {
				putReplica.push_back(p);
			}
			
		}
		while (deleteReplica.size() > 0) {
			std::pair<std::string, std::string> p = deleteReplica.back();
			deleteReplica.pop_back();
			std::string replica_addr;
			master.GetReplica(p.first, p.second, replica_addr);
			StorageClient replica(grpc::CreateChannel(replica_addr, grpc::InsecureChannelCredentials()));		
			if (replica.Ping()) {
				std::string val;
				replica.Delete(p.first, p.second);
			} else {
				deleteReplica.push_back(p);
			}
		}
	}	
}

/* get data when initializing the node*/
void* get_data(void*) {
	sleep(5);
	if (master.Ping()) {
		wLogger.log_trace("master is ready, requesting data");
		std::vector<std::pair<std::string, std::string> > pairs;
		if (master.AddNode(worker_addr, pairs)) {
			for (std::pair<std::string, std::string> p : pairs) {


				std::string other = get_real_addr(p.first);
				StorageClient worker(grpc::CreateChannel(other, grpc::InsecureChannelCredentials()));
				StorageClient self(grpc::CreateChannel(worker_addr, grpc::InsecureChannelCredentials()));

				std::unordered_map<std::string, std::unordered_map<std::string, std::string> > data;

				// (other + self)
				if (worker.Migrate(p.first + " " + p.second, data)) {
					wLogger.log_trace("migrating data from " + other);
					for (auto it = data.begin(); it != data.end(); it++) {
						for (auto itr = it->second.begin(); itr != it->second.end(); itr++) {
							self.Put(it->first, itr->first, itr->second);
						}
					}
				}
			}
			wLogger.log_trace("migrating done");
		}

	} else {
		wLogger.log_error("master is down");
	}
}



void RunServer() {

	// std::string server_address(WORKER_ADDR);
	std::string server_address(worker_addr);
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

/* hanler for "contrl + C" */
// void sigint(int a) {
//     wLogger.log_trace("Node is about to exit.");
//     std::vector<std::string> nodes;
//     master.GetNode(worker_addr, "", nodes);
// }



int main(int argc, char** argv) {
	if(argc != 2) {
		wLogger.log_error("Wrong configuration, please enter a port number.");
		return 0;
	}

	// signal(SIGINT, sigint);

	worker_addr.append(argv[1]);

	wLogger.log_config("StorageServer");
	pthread_t server_thread;
	if (0 != pthread_create(&server_thread, NULL, &get_data, NULL)) {
		wLogger.log_error("failed to start thread to get data");
	}

	pthread_t backup_thread;
	if (0 != pthread_create(&backup_thread, NULL, &replay_replica, NULL)) {
		wLogger.log_error("failed to start thread to replay replica");
	}

	RunServer();

  	return 0;
}
