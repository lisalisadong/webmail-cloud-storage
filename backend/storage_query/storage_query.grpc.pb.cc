// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: storage_query.proto

#include "storage_query.pb.h"
#include "storage_query.grpc.pb.h"

#include <grpc++/impl/codegen/async_stream.h>
#include <grpc++/impl/codegen/async_unary_call.h>
#include <grpc++/impl/codegen/channel_interface.h>
#include <grpc++/impl/codegen/client_unary_call.h>
#include <grpc++/impl/codegen/method_handler_impl.h>
#include <grpc++/impl/codegen/rpc_service_method.h>
#include <grpc++/impl/codegen/service_type.h>
#include <grpc++/impl/codegen/sync_stream.h>
namespace storagequery {

static const char* StorageQuery_method_names[] = {
  "/storagequery.StorageQuery/Get",
  "/storagequery.StorageQuery/Put",
  "/storagequery.StorageQuery/CPut",
  "/storagequery.StorageQuery/Delete",
  "/storagequery.StorageQuery/Migrate",
};

std::unique_ptr< StorageQuery::Stub> StorageQuery::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  std::unique_ptr< StorageQuery::Stub> stub(new StorageQuery::Stub(channel));
  return stub;
}

StorageQuery::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Get_(StorageQuery_method_names[0], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Put_(StorageQuery_method_names[1], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_CPut_(StorageQuery_method_names[2], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Delete_(StorageQuery_method_names[3], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Migrate_(StorageQuery_method_names[4], ::grpc::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status StorageQuery::Stub::Get(::grpc::ClientContext* context, const ::storagequery::GetRequest& request, ::storagequery::GetResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_Get_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::storagequery::GetResponse>* StorageQuery::Stub::AsyncGetRaw(::grpc::ClientContext* context, const ::storagequery::GetRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::storagequery::GetResponse>(channel_.get(), cq, rpcmethod_Get_, context, request);
}

::grpc::Status StorageQuery::Stub::Put(::grpc::ClientContext* context, const ::storagequery::PutRequest& request, ::storagequery::PutResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_Put_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::storagequery::PutResponse>* StorageQuery::Stub::AsyncPutRaw(::grpc::ClientContext* context, const ::storagequery::PutRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::storagequery::PutResponse>(channel_.get(), cq, rpcmethod_Put_, context, request);
}

::grpc::Status StorageQuery::Stub::CPut(::grpc::ClientContext* context, const ::storagequery::CPutRequest& request, ::storagequery::CPutResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_CPut_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::storagequery::CPutResponse>* StorageQuery::Stub::AsyncCPutRaw(::grpc::ClientContext* context, const ::storagequery::CPutRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::storagequery::CPutResponse>(channel_.get(), cq, rpcmethod_CPut_, context, request);
}

::grpc::Status StorageQuery::Stub::Delete(::grpc::ClientContext* context, const ::storagequery::DeleteRequest& request, ::storagequery::DeleteResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_Delete_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::storagequery::DeleteResponse>* StorageQuery::Stub::AsyncDeleteRaw(::grpc::ClientContext* context, const ::storagequery::DeleteRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::storagequery::DeleteResponse>(channel_.get(), cq, rpcmethod_Delete_, context, request);
}

::grpc::Status StorageQuery::Stub::Migrate(::grpc::ClientContext* context, const ::storagequery::MigrateRequest& request, ::storagequery::MigrateResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_Migrate_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::storagequery::MigrateResponse>* StorageQuery::Stub::AsyncMigrateRaw(::grpc::ClientContext* context, const ::storagequery::MigrateRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::storagequery::MigrateResponse>(channel_.get(), cq, rpcmethod_Migrate_, context, request);
}

StorageQuery::Service::Service() {
  (void)StorageQuery_method_names;
  AddMethod(new ::grpc::RpcServiceMethod(
      StorageQuery_method_names[0],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< StorageQuery::Service, ::storagequery::GetRequest, ::storagequery::GetResponse>(
          std::mem_fn(&StorageQuery::Service::Get), this)));
  AddMethod(new ::grpc::RpcServiceMethod(
      StorageQuery_method_names[1],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< StorageQuery::Service, ::storagequery::PutRequest, ::storagequery::PutResponse>(
          std::mem_fn(&StorageQuery::Service::Put), this)));
  AddMethod(new ::grpc::RpcServiceMethod(
      StorageQuery_method_names[2],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< StorageQuery::Service, ::storagequery::CPutRequest, ::storagequery::CPutResponse>(
          std::mem_fn(&StorageQuery::Service::CPut), this)));
  AddMethod(new ::grpc::RpcServiceMethod(
      StorageQuery_method_names[3],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< StorageQuery::Service, ::storagequery::DeleteRequest, ::storagequery::DeleteResponse>(
          std::mem_fn(&StorageQuery::Service::Delete), this)));
  AddMethod(new ::grpc::RpcServiceMethod(
      StorageQuery_method_names[4],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< StorageQuery::Service, ::storagequery::MigrateRequest, ::storagequery::MigrateResponse>(
          std::mem_fn(&StorageQuery::Service::Migrate), this)));
}

StorageQuery::Service::~Service() {
}

::grpc::Status StorageQuery::Service::Get(::grpc::ServerContext* context, const ::storagequery::GetRequest* request, ::storagequery::GetResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status StorageQuery::Service::Put(::grpc::ServerContext* context, const ::storagequery::PutRequest* request, ::storagequery::PutResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status StorageQuery::Service::CPut(::grpc::ServerContext* context, const ::storagequery::CPutRequest* request, ::storagequery::CPutResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status StorageQuery::Service::Delete(::grpc::ServerContext* context, const ::storagequery::DeleteRequest* request, ::storagequery::DeleteResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status StorageQuery::Service::Migrate(::grpc::ServerContext* context, const ::storagequery::MigrateRequest* request, ::storagequery::MigrateResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace storagequery

