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

#define INPUT_SIZE 10;


int main() {
	/* start the master */
	MasterClient master(grpc::CreateChannel("localhost:8000", grpc::InsecureChannelCredentials()));
	std::string addr1;
	std::string addr2;

	std::vector<std::string> vec;
	// master.GetNode("r1", "c1", vec);

	// if(vec.size() == 0) {
	// 	std::cout << "No node found." << std::endl;
	// 	return 0;
	// } else {
	// 	addr1 = vec[0];
	// }

	// StorageClient s1(grpc::CreateChannel(addr1, grpc::InsecureChannelCredentials()));
	std::string row; 
	std::string col; 
	std::string val; 
	for(int i = 0; i < 100; i++) {
		row = "r" + std::to_string(i);
		col = "c" + std::to_string(i);
		val = "v" + std::to_string(i);
		// s1.Put(row, col, val);
		master.GetNode(row, val, vec);

		std::cout << "(" << row << ", " << col << ") is in node: " << vec[0] << std::endl;
	}

	// for(int i = 0; i < 10; i++) {
	// 	row = "r" + std::to_string(i);
	// 	col = "c" + std::to_string(i);
	// 	std::cout << "Got (" << row << ", " << col << "): ";
	// 	if(s1.Get(row, col, val)) {
	// 		std::cout << val << std::endl;
	// 	} else {
	// 		std::cout << "Key not found." << std::endl;
	// 	}
	// }

	return 0;
}