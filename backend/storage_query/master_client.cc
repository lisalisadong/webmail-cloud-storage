#include <iostream>
#include "master_client.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using storagequery::StorageQuery;
using storagequery::GetRequest;
using storagequery::GetResponse;
using storagequery::AddNodeRequest;
using storagequery::AddNodeResponse;
using storagequery::DeleteNodeRequest;
using storagequery::DeleteNodeResponse;
using storagequery::MigrateRequest;
using storagequery::MigrateResponse;
using storagequery::PingRequest;
using storagequery::PingResponse;

bool MasterClient::Get(const std::string& row, const std::string& col, const std::string& addr) {
  // Data we are sending to the server.
  GetRequest request;
  request.set_row(row);
  request.set_col(col);

  // Container for the data we expect from the server.
  GetResponse response;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->Get(&context, request, &response);

  // Act upon its status.
  if (status.ok()) {
    addr = response.val();
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

bool MasterClient::AddNode(const std::string& addr, std::vector<std::pair<std::string, std::string> >& val) {
	// Data we are sending to the server.
	AddNodeRequest request;
	request.set_addr(addr);

	// Container for the data we expect from the server.
	AddNodeResponse response;

	// Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
	ClientContext context;

	// The actual RPC.
  Status status = stub_->AddNode(&context, request, &response);
Get
  // Act upon its status.
  if (status.ok()) {
    std::string dataMap = response.data_map();
    //for testing
    val.push_back(std::make_pair("localhost:50051_2", "localhost:50051_1"));
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

bool MasterClient::DeleteNode(const std::string& addr) {
	// Data we are sending to the server.
	DeleteNodeRequest request;
	request.set_addr(row);

	// Container for the data we expect from the server.
	DeleteNodeResponse response;

	// Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
	ClientContext context;

	// The actual RPC.
  Status status = stub_->DeleteNode(&context, request, &response);

  // Act upon its status.
  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

bool MasterClient::Ping() {
  ClientContext context;
  PingRequest request;
  PingResponse response;
  Status status = stub_->Ping(&context, request, &response);
  return status.ok();
}