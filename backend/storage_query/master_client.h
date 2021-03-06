#ifndef M_C_
#define M_C_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "storage_query.grpc.pb.h"
#include "logger.h"

using grpc::Channel;
using storagequery::StorageQuery;
using grpc::ClientContext;
using grpc::Status;
using storagequery::StorageQuery;

class MasterClient {
public:
	MasterClient(std::shared_ptr<Channel> channel) :
			stub_(StorageQuery::NewStub(channel)) {logger.log_config("MasterClient");}

	bool GetNode(const std::string& row, const std::string& col, std::vector<std::string>& addr);

	bool GetReplica(const std::string& row, const std::string& col, std::string& addr);

	bool AddNode(const std::string& addr, std::vector<std::pair<std::string, std::string> >& val);

	bool Ping();

	bool GetAllNodes(std::vector<std::string>& ups, std::vector<std::string>& downs);

private:
	Logger logger;
	std::unique_ptr<StorageQuery::Stub> stub_;

	std::pair<std::string, std::string> getPair(std::string str);

	void deserialize(std::vector<std::pair<std::string, std::string> >& pairs, std::string nodes);

	void getNodes(std::vector<std::string>& addr, std::string str);

};

// std::string StorageClient::Get(const std::string& row, const std::string& col) {
// 	// Data we are sending to the server.
// 	GetRequest request;
// 	request.set_row(row);
// 	request.set_col(col);

// 	// Container for the data we expect from the server.
// 	GetResponse response;

// 	// Context for the client. It could be used to convey extra information to
// 	// the server and/or tweak certain RPC behaviors.
// 	ClientContext context;

// 	// The actual RPC.
// 	Status status = stub_->Get(&context, request, &response);

// 	// Act upon its status.
// 	if (status.ok()) {
// 		return response.val();
// 	} else {
// 		std::cout << status.error_code() << ": " << status.error_message()
// 				<< std::endl;
// 		return "Not Found!";
// 	}
// }

// void StorageClient::Put(const std::string& row, const std::string& col,
// 		const std::string& val) {
// 	// Data we are sending to the server.
// 	PutRequest request;
// 	request.set_row(row);
// 	request.set_col(col);
// 	request.set_val(val);

// 	// Container for the data we expect from the server.
// 	PutResponse response;

// 	// Context for the client. It could be used to convey extra information to
// 	// the server and/or tweak certain RPC behaviors.
// 	ClientContext context;

// 	// The actual RPC.
// 	Status status = stub_->Put(&context, request, &response);

// 	// Act upon its status.
// 	if (status.ok()) {
// 		return;
// 	} else {
// 		std::cout << status.error_code() << ": " << status.error_message()
// 				<< std::endl;
// 		return;
// 	}
// }

// void StorageClient::CPut(const std::string& row, const std::string& col,
// 		const std::string& val1, const std::string& val2) {
// 	// Data we are sending to the server.
// 	CPutRequest request;
// 	request.set_row(row);
// 	request.set_col(col);
// 	request.set_val1(val1);
// 	request.set_val2(val2);

// 	// Container for the data we expect from the server.
// 	CPutResponse response;

// 	// Context for the client. It could be used to convey extra information to
// 	// the server and/or tweak certain RPC behaviors.
// 	ClientContext context;

// 	// The actual RPC.
// 	Status status = stub_->CPut(&context, request, &response);

// 	// Act upon its status.
// 	if (status.ok()) {
// 		return;
// 	} else {
// 		std::cout << status.error_code() << ": " << status.error_message()
// 				<< std::endl;
// 		return;
// 	}
// }

// void StorageClient::Delete(const std::string& row, const std::string& col) {
// 	// Data we are sending to the server.
// 	DeleteRequest request;
// 	request.set_row(row);
// 	request.set_col(col);

// 	// Container for the data we expect from the server.
// 	DeleteResponse response;

// 	// Context for the client. It could be used to convey extra information to
// 	// the server and/or tweak certain RPC behaviors.
// 	ClientContext context;

// 	// The actual RPC.
// 	Status status = stub_->Delete(&context, request, &response);

// 	// Act upon its status.
// 	if (status.ok()) {
// 		return;
// 	} else {
// 		std::cout << status.error_code() << ": " << status.error_message()
// 				<< std::endl;
// 		return;
// 	}
// }

#endif