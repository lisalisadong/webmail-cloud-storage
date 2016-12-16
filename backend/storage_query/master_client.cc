#include <iostream>
#include "master_client.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using storagequery::StorageQuery;
using storagequery::GetNodeRequest;
using storagequery::GetNodeResponse;
using storagequery::AddNodeRequest;
using storagequery::AddNodeResponse;
using storagequery::GetReplicaRequest;
using storagequery::GetReplicaResponse;
using storagequery::PingRequest;
using storagequery::PingResponse;

std::pair<std::string, std::string> getPair(std::string str);
void deserialize(std::vector<std::pair<std::string, std::string> >& pairs, std::string nodes);
void getNodes(std::vector<std::string>& addr, std::string str);

// addr should be a vector or set of addr?
bool MasterClient::GetNode(const std::string& row, const std::string& col, std::vector<std::string>& addr) {
  // Data we are sending to the server.
  GetNodeRequest request;
  request.set_row(row);
  request.set_col(col);

  // Container for the data we expect from the server.
  GetNodeResponse response;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->GetNode(&context, request, &response);

  // Act upon its status.
  if (status.ok()) {
    std::string s = response.addr();

    getNodes(addr, s);

    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

bool MasterClient::GetReplica(const std::string& row, const std::string& col, std::string& addr) {
  // Data we are sending to the server.
  GetReplicaRequest request;
  request.set_row(row);
  request.set_col(col);

  // Container for the data we expect from the server.
  GetReplicaResponse response;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->GetReplica(&context, request, &response);

  // Act upon its status.
  if (status.ok()) {
    addr = response.addr();
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

  // Act upon its status.
  if (status.ok()) {
    std::string dataMap = response.data_map();
    //for testing
    // val.push_back(std::make_pair("localhost:50051_2", "localhost:50051_1"));

    deserialize(val, dataMap);

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

void getNodes(std::vector<std::string>& addr, std::string str) {
  int start = 0;
  for(int i = 0; i < str.length(); i++) {
    if(str[i] != ' ') continue;
    addr.push_back(str.substr(start, i - start));
    start = i + 1;
  }
}

// int main(int argc, char** argv) {
	// TODO:
	// Instantiate the client. It requires a channel, out of which the actual RPCs
 // are created. This channel models a connection to an endpoint (in this case,
 // localhost at port 50051). We indicate that the channel isn't authenticated
 // (use of InsecureChannelCredentials()).
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
 //  std::vector<std::pair<std::string, std::string> > pairs;
 //  deserialize(pairs, "a1 b1,a2 b2,");

// }
