#ifndef S_C_
#define S_C_

#include <memory>
#include <string>
#include <unordered_map>
#include <grpc++/grpc++.h>
#include "storage_query.grpc.pb.h"
#include "logger.h"

using grpc::Channel;
using storagequery::StorageQuery;
using grpc::ClientContext;
using grpc::Status;
using storagequery::StorageQuery;
using storagequery::GetRequest;
using storagequery::GetResponse;
using storagequery::PutRequest;
using storagequery::PutResponse;
using storagequery::CPutRequest;
using storagequery::CPutResponse;
using storagequery::DeleteRequest;
using storagequery::DeleteResponse;

class StorageClient {
public:
	StorageClient(std::shared_ptr<Channel> channel) :
			stub_(StorageQuery::NewStub(channel)) {logger.log_config("StorageClient");}

	bool Get(const std::string& row, const std::string& col, std::string& val);

	bool Put(const std::string& row, const std::string& col,
			const std::string& val);

	bool CPut(const std::string& row, const std::string& col,
			const std::string& val1, const std::string& val2);

	bool Delete(const std::string& row, const std::string& col);

	bool Ping();

	bool Migrate(std::string virtualAddr, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& data);

	int GetData(int start, int size, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& data);

private:
	Logger logger;
	std::unique_ptr<StorageQuery::Stub> stub_;

	void deserialize_data_to_map(std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& data, std::string rawData);

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