#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "storage_query.grpc.pb.h"
#include "storage_client.h"

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

std::string StorageClient::Get(const std::string& row, const std::string& col) {
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
    return response.val();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "Not Found!";
  }
}

void StorageClient::Put(const std::string& row, const std::string& col, const std::string& val) {
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
    return;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return;
  }
}

void StorageClient::CPut(const std::string& row, const std::string& col, 
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
    return;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return;
  }
}

void StorageClient::Delete(const std::string& row, const std::string& col) {
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
    return;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return;
  }
}

int main(int argc, char** argv) {	
	// TODO:
	// Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  StorageClient client(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

//   // while(1) {
    
//   //   std::string mode;

//   //   std::cout << "Enter mode: " << std::endl;
    
//   //   std::cin >> mode;

//   //   if(mode == "g") {
//   //     std::string row;
//   //     std::cout << "Enter row: " << std::endl;
//   //     std::cin >> row;
      
//   //     std::string col;
//   //     std::cout << "Enter col: " << std::endl;
//   //     std::cin >> col;

//   //     std::string res = client.Get(row, col);

//   //     std::cout << "Reponse: " << res << std::endl;
//   //   } else {
//   //     std::string row;
//   //     std::cout << "Enter row: " << std::endl;
//   //     std::cin >> row;
      
//   //     std::string col;
//   //     std::cout << "Enter col: " << std::endl;
//   //     std::cin >> col;

//   //     std::string val;
//   //     std::cout << "Enter val: " << std::endl;
//   //     std::cin >> val;

//   //     client.Put(row, col, val);

//   //   }
//   // }


  std::string response = client.Get("r1", "c1");
  std::cout << "getting r1||c1: " << response << std::endl;

  client.Put("r1", "c1", "v1");
  std::cout << "put r1||c1||v1" << std::endl;

  response = client.Get("r1", "c1");
  std::cout << "getting r1||c1: " << response << std::endl;

  std::cout << "=================================" << std::endl;
  std::cout  << std::endl;

  client.Put("r2", "c2", "v2");
  std::cout << "put r2||c2||v2" << std::endl;
  
  response = client.Get("r2", "c2");
  std::cout << "getting r2||c2: " << response << std::endl;

  response = client.Get("r2", "c2");
  std::cout << "getting r2||c2: " << response << std::endl;

  std::cout << "=================================" << std::endl;
  std::cout  << std::endl;

  client.Put("r3", "c3", "v3");
  std::cout << "put r3||c3||v3" << std::endl;
  
  response = client.Get("r3", "c3");
  std::cout << "getting r3||c3: " << response << std::endl;

  std::cout << "=================================" << std::endl;
  std::cout  << std::endl;

  client.Delete("r1", "c1");
  std::cout << "delete r1||c1||v1" << std::endl;

  response = client.Get("r1", "c1");
  std::cout << "getting r1||c1: " << response << std::endl;



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

  return 0;
}