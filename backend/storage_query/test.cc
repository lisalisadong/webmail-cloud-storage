#include "storage_client.h"
#include "master_client.h"
#include <string>
#include <iostream>

#include "storage_query.grpc.pb.h"
#include <grpc++/grpc++.h>

using grpc::Channel;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using storagequery::StorageQuery;

int main() {
	MasterClient master(grpc::CreateChannel("localhost:8000", grpc::InsecureChannelCredentials()));
	std::cout << "test" << std::endl;
	return 0;
}