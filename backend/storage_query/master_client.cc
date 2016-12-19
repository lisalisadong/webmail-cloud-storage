#include <iostream>
#include "master_client.h"
#include "utils.h"

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
using storagequery::GetAllNodesRequest;
using storagequery::GetAllNodesResponse;

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
  addr.clear();

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
  val.clear();

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

bool MasterClient::GetAllNodes(std::vector<std::string>& ups, std::vector<std::string>& downs) {
  ClientContext context;
  GetAllNodesRequest request;
  GetAllNodesResponse response;
  Status status = stub_->GetAllNodes(&context, request, &response);
  std::string up = response.up();
  std::string down = response.down();
  ups.clear();
  downs.clear();
  int pos = 0;
  while (pos < up.length()) {
    ups.push_back(deserialize_next(up, pos));
  }
  pos = 0;
  while (pos < down.length()) {
    downs.push_back(deserialize_next(down, pos));
  }
  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

std::pair<std::string, std::string> MasterClient::getPair(std::string str) {
  int i = 0;
  while(str[i] != ' ') { i++; }

  std::cout << "(" << str.substr(0, i) << ", " << str.substr(i + 1, str.length() - i) << ")" << std::endl;

  return std::make_pair(str.substr(0, i), str.substr(i + 1, str.length() - i));
}

void MasterClient::deserialize(std::vector<std::pair<std::string, std::string> >& pairs, std::string nodes) {
  int start = 0, end = 0;
  for (int end = 0; end < nodes.length(); end++) {
    if(nodes[end] != ',') continue;
    std::string pair = nodes.substr(start, end - start);
    start = end + 1;

    pairs.push_back(getPair(pair));

  }
}

void MasterClient::getNodes(std::vector<std::string>& addr, std::string str) {
  addr.clear();

  int found = 0;

  while(found < str.length() && str[found] != ' ') found++;

  // std::cout << "First: " << str.substr(0, found) << std::endl;

  if (found == 0) return;

  addr.push_back(str.substr(0, found));

  if(found < str.length() - 1) {
    // std::cout << "Second: " << str.substr(found + 1, str.length() - found - 1) << std::endl;
    addr.push_back(str.substr(found + 1, str.length() - found - 1));
  }
}

// int main(int argc, char** argv) {
// 	// TODO:
// 	// Instantiate the client. It requires a channel, out of which the actual RPCs
//  // are created. This channel models a connection to an endpoint (in this case,
//  // localhost at port 50051). We indicate that the channel isn't authenticated
//  // (use of InsecureChannelCredentials()).
//   Logger logger;
//   logger.log_config("StorageClient");

//   std::string serverAddr = "localhost:8000";

//   MasterClient client(grpc::CreateChannel(
//   serverAddr, grpc::InsecureChannelCredentials()));

//   //=====================test======================
//   std::vector<std::string> addr;
//   if(client.GetNode("r1", "c1", addr)) {
//     if(addr.size() == 0) {
//       logger.log_trace("No such key (r1, c1).");
//     } else {
//       logger.log_trace("Addr for r1, c1: ");
//       for(std::string n: addr) {
//         logger.log_trace(n);
//       }
//     }
//   } else {
//     logger.log_error("Something's wrong.");
//   }

// }




















