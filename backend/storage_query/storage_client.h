#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "storage_query.grpc.pb.h"

using grpc::Channel;
using storagequery::StorageQuery;

class StorageClient {
public:
	StorageClient(std::shared_ptr<Channel> channel)
		: stub_(StorageQuery::NewStub(channel)) {}

	std::string Get(const std::string& row, const std::string& col);

	void Put(const std::string& row, const std::string& col, const std::string& val);

	void CPut(const std::string& row, const std::string& col, 
						const std::string& val1, const std::string& val2);

	void Delete(const std::string& row, const std::string& col);

private:
  std::unique_ptr<StorageQuery::Stub> stub_;
	
};