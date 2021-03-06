#include <iostream>
#include "storage_client.h"
#include "utils.h"

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
using storagequery::GetDataRequest;
using storagequery::GetDataResponse;

bool StorageClient::Get(const std::string& row, const std::string& col, std::string& val) {
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
    val = response.val();
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

bool StorageClient::Put(const std::string& row, const std::string& col, const std::string& val) {
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

bool StorageClient::CPut(const std::string& row, const std::string& col, 
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

bool StorageClient::Delete(const std::string& row, const std::string& col) {
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

bool StorageClient::Ping() {
  ClientContext context;
  PingRequest request;
  PingResponse response;
  Status status = stub_->Ping(&context, request, &response);
  return status.ok();
}

bool StorageClient::Migrate(std::string virtualAddr, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& data) {
  ClientContext context;
  MigrateRequest request;
  MigrateResponse response;
  request.set_addr(virtualAddr);

  Status status = stub_->Migrate(&context, request, &response);

  data.clear();
  deserialize_data_to_map(data, response.data());

  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

int StorageClient::GetData(int start, int size, std::map<std::string, std::map<std::string, std::string> >& data) {
  ClientContext context;
  GetDataRequest request;
  GetDataResponse response;
  request.set_start(start);
  request.set_size(size);
  Status status = stub_->GetData(&context, request, &response);
  data.clear();
  deserialize_data_to_map(data, response.data());
  if (status.ok()) {
    return response.size();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return 0;
  }
  
}


void StorageClient::deserialize_data_to_map(std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& data, std::string rawData) {
  int ePos = 0;
  while (ePos < rawData.length()) {
    std::string entry = deserialize_next(rawData, ePos);
    int pos = 0;
    std::string row = deserialize_next(entry, pos);
    std::string col = deserialize_next(entry, pos);
    std::string val = deserialize_next(entry, pos);
    data[row][col] = val;
  }
}

void StorageClient::deserialize_data_to_map(std::map<std::string, std::map<std::string, std::string> >& data, std::string rawData) {
  int ePos = 0;
  while (ePos < rawData.length()) {
    std::string entry = deserialize_next(rawData, ePos);
    int pos = 0;
    std::string row = deserialize_next(entry, pos);
    std::string col = deserialize_next(entry, pos);
    std::string val = deserialize_next(entry, pos);
    data[row][col] = val;
  }
}



// int main(int argc, char** argv) {
// 	// TODO:
// 	// Instantiate the client. It requires a channel, out of which the actual RPCs
// //  are created. This channel models a connection to an endpoint (in this case,
// //  localhost at port 50051). We indicate that the channel isn't authenticated
// //  (use of InsecureChannelCredentials()).
//   Logger logger;
//   logger.log_config("StorageClient");

//   std::string serverAddr = "localhost:50051";

//  StorageClient client(grpc::CreateChannel(
//      serverAddr, grpc::InsecureChannelCredentials()));

//  if (client.Ping()) {
//   logger.log_trace("Channel created. Server " + serverAddr + " is ready to accept rpcs.");
//  } else {
//   logger.log_warn("Channel cannot be created. Server " + serverAddr + " is down.");
//   // rehashing
//   return 0;
//  }

//  // StorageClient client(grpc::CreateChannel(
//  //     "localhost:8000", grpc::InsecureChannelCredentials()));

//  std::string response = client.Get("1", "csadas");
//  std::cout << "getting address 1: " << response << std::endl;
//  // response = client.Get("r1", "dasdasfsa");
//  // std::cout << "getting address 2: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  client.Put("r1", "c1", "v1");
//  std::cout << "put r1||c1||v1" << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//  client.Put("r2", "c2", "v2");
//  std::cout << "put r2||c2||v2" << std::endl;

//  response = client.Get("r2", "c2");
//  std::cout << "getting r2||c2: " << response << std::endl;

//  response = client.Get("r2", "c2");
//  std::cout << "getting r2||c2: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//  client.Put("r3", "c3", "v3");
//  std::cout << "put r3||c3||v3" << std::endl;

//  response = client.Get("r3", "c3");
//  std::cout << "getting r3||c3: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//  client.Put("r4", "c4", "v4");
//  std::cout << "put r4||c4||v4" << std::endl;

//  response = client.Get("r4", "c4");
//  std::cout << "getting r4||c4: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//  client.Put("r5", "c5", "v5");
//  std::cout << "put r5||c5||v5" << std::endl;

//  response = client.Get("r5", "c5");
//  std::cout << "getting r5||c5: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//  client.Put("r6", "c6", "v6");
//  std::cout << "put r6||c6||v6" << std::endl;

//  response = client.Get("r6", "c6");
//  std::cout << "getting r6||c6: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//  client.Put("r7", "c7", "v7");
//  std::cout << "put r7||c7||v7" << std::endl;

//  response = client.Get("r7", "c7");
//  std::cout << "getting r7||c7: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//  client.Put("r8", "c8", "v8");
//  std::cout << "put r8||c8||v8" << std::endl;

//  response = client.Get("r8", "c8");
//  std::cout << "getting r8||c8: " << response << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//    client.Put("r9", "c9", "v9");
//  std::cout << "put r6||c6||v6" << std::endl;

//  response = client.Get("r9", "c9");
//  std::cout << "getting r6||c6: " << response << std::endl;

//  response = client.Get("r2", "c2");
//  std::cout << "getting r2||c2: " << response << std::endl;

//  std::cout << "=================================" << std::endl;
//  std::cout  << std::endl;

//  client.Delete("r1", "c1");
//  std::cout << "delete r1||c1||v1" << std::endl;

//  response = client.Get("r1", "c1");
//  std::cout << "getting r1||c1: " << response << std::endl;
// }

  // client.CPut(row, col, "val", "abc");
  // std::cout << "c putting lisa||emails||abc" << std::endl;

  // std::cout << "=================================" << std::endl;
  // std::cout  << std::endl;

  // response = client.Get(row, col);
  // std::cout << "getting lisa||emails: " << response << std::endl;

  // std::cout << "=================================" << std::endl;
  // std::cout  << std::endl;

  // client.CPut(row, col, "from 1 to 2:xxx", "abc");
  // std::cout << "c putting lisa||emails||abc" << std::endl;

  // std::cout << "=================================" << std::endl;
  // std::cout  << std::endl;

  // response = client.Get(row, col);
  // std::cout << "getting lisa||emails: " << response << std::endl;

  // std::cout << "=================================" << std::endl;
  // std::cout  << std::endl;


  // response = client.Get(row, col);
  // std::cout << "getting lisa||emails: " << response << std::endl;


  // row = "Tom"; col = "Cruise"; val = "wow";
  // client.Put(row, col, val);
  // std::cout << "putting Tom||Cruise||wow" << std::endl;

  // response = client.Get(row, col);
  // std::cout << "getting Tom||Cruise: " << response << std::endl;

  

//  //  std::string val_new("from 1 to 2:ooo");
//  //  client.CPut(row, col, val_new, val_new);
//  //  std::cout << "cputting lisa||emails||from 1 to 2:ooo||from 1 to 2:ooo" << std::endl;

//  //  response = client.Get(row, col);
//  //  std::cout << "getting lisa||emails: " << response << std::endl;

//  //  client.CPut(row, col, val, val_new);
//  //  std::cout << "cputting lisa||emails||from 1 to 2:xxx||from 1 to 2:ooo" << std::endl;

//  //  response = client.Get(row, col);
//  //  std::cout << "getting lisa||emails: " << response << std::endl;

//  //  client.Delete(row, col);
//  //  std::cout << "deleting lisa||emails: " << response << std::endl;

// 	// response = client.Get(row, col);
//  //  std::cout << "getting lisa||emails: " << response << std::endl;

//   return 0;
// }

// }
