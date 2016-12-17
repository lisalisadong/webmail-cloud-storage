#include "storage_client.h"
#include "master_client.h"
#include <string>
#include <iostream>
#include <vector>

#include "storage_query.grpc.pb.h"
#include <grpc++/grpc++.h>

using grpc::Channel;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using storagequery::StorageQuery;

int main() {
	MasterClient master(grpc::CreateChannel("localhost:8000", grpc::InsecureChannelCredentials()));
	std::vector<std::string> addrs;
	master.GetNode("r1", "c1", addrs);
	std::cout << addrs.at(0) << std::endl;
	StorageClient client(grpc::CreateChannel(addrs.at(0), grpc::InsecureChannelCredentials()));
	client.Put("r1", "c1", "v1");

	addrs.clear();
	master.GetNode("r1", "c1", addrs);
	std::cout << addrs.at(0) << std::endl;
	StorageClient client2(grpc::CreateChannel(addrs.at(0), grpc::InsecureChannelCredentials()));
	std::string val;
	client2.Get("r1", "c1", val);
	std::cout << val << std::endl;
	return 0;
}