#include <iostream>
#include "master_client.h"

using grpc::Channel;
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
using storagequery::MigrateRequest;
using storagequery::MigrateResponse;
using storagequery::PingRequest;
using storagequery::PingResponse;

bool MasterClient::Get(const std::string& row, const std::string& col, std::vector<std::pair<std::string, std::string> >& val) {
	// Data we are sending to the server.
	GetRequest request;
	request.set_row(row);
	//request.set_col(col);

	// Container for the data we expect from the server.
	GetResponse response;

	// Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
	ClientContext context;

	// The actual RPC.
  Status status = stub_->Get(&context, request, &response);

  // Act upon its status.
  if (status.ok()) {
    //for testing
    val.push_back(std::make_pair("localhost:50051_2", "localhost:50051_1"));
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

bool MasterClient::Put(const std::string& row, const std::string& col, const std::string& val) {
	// Data we are sending to the server.
	PutRequest request;
	request.set_row(row);
	request.set_col(col);
	request.set_val(val);

	// Container for the data we expect from the server.
	PutResponse response;

	// Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
	ClientContext context;

	// The actual RPC.
  Status status = stub_->Put(&context, request, &response);

  // Act upon its status.
  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

bool MasterClient::CPut(const std::string& row, const std::string& col, 
					const std::string& val1, const std::string& val2) {
	// Data we are sending to the server.
	CPutRequest request;
	request.set_row(row);
	request.set_col(col);
	request.set_val1(val1);
	request.set_val2(val2);

	// Container for the data we expect from the server.
	CPutResponse response;

	// Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
	ClientContext context;

	// The actual RPC.
  Status status = stub_->CPut(&context, request, &response);

  // Act upon its status.
  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

bool MasterClient::Delete(const std::string& row, const std::string& col) {
	// Data we are sending to the server.
	DeleteRequest request;
	request.set_row(row);
	request.set_col(col);

	// Container for the data we expect from the server.
	DeleteResponse response;

	// Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
	ClientContext context;

	// The actual RPC.
  Status status = stub_->Delete(&context, request, &response);

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

std::pair<std::string, std::string> getPair(std::string str) {
  int i = 0;
  while(str[i] != ' ') { i++; }

  std::cout << "(" << str.substr(0, i) << ", " << str.substr(i + 1, str.length() - i) << ")" << std::endl;

  return std::make_pair(str.substr(0, i), str.substr(i + 1, str.length() - i));
}

void deserialize(std::vector<std::pair<std::string, std::string> >& pairs, std::string nodes) {
  int start = 0, end = 0;
  for (int end = 0; end < nodes.length(); end++) {
    if(nodes[end] != ',') continue;
    std::string pair = nodes.substr(start, end - start);
    start = end + 1;

    pairs.push_back(getPair(pair));

  }
}

// int main(int argc, char** argv) {
	// TODO:
	// Instantiate the client. It requires a channel, out of which the actual RPCs
//  are created. This channel models a connection to an endpoint (in this case,
//  localhost at port 50051). We indicate that the channel isn't authenticated
//  (use of InsecureChannelCredentials()).
 //  Logger logger;
 //  logger.log_config("StorageClient");

 //  std::string serverAddr = "localhost:50051";

 // StorageClient client(grpc::CreateChannel(
 //     serverAddr, grpc::InsecureChannelCredentials()));

 // if (client.Ping()) {
 //  logger.log_trace("Channel created. Server " + serverAddr + " is ready to accept rpcs.");
 // } else {
 //  logger.log_warn("Channel cannot be created. Server " + serverAddr + " is down.");
 //  // rehashing
 //  return 0;
 // }
  // std::vector<std::pair<std::string, std::string> > pairs;
  // deserialize(pairs, "a1 b1,a2 b2,");

// }
