// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: storage_query.proto
#ifndef GRPC_storage_5fquery_2eproto__INCLUDED
#define GRPC_storage_5fquery_2eproto__INCLUDED

#include "storage_query.pb.h"

#include <grpc++/impl/codegen/async_stream.h>
#include <grpc++/impl/codegen/async_unary_call.h>
#include <grpc++/impl/codegen/proto_utils.h>
#include <grpc++/impl/codegen/rpc_method.h>
#include <grpc++/impl/codegen/service_type.h>
#include <grpc++/impl/codegen/status.h>
#include <grpc++/impl/codegen/stub_options.h>
#include <grpc++/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class RpcService;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace storagequery {

class StorageQuery GRPC_FINAL {
 public:
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status Get(::grpc::ClientContext* context, const ::storagequery::GetRequest& request, ::storagequery::GetResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::GetResponse>> AsyncGet(::grpc::ClientContext* context, const ::storagequery::GetRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::GetResponse>>(AsyncGetRaw(context, request, cq));
    }
    virtual ::grpc::Status Put(::grpc::ClientContext* context, const ::storagequery::PutRequest& request, ::storagequery::PutResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::PutResponse>> AsyncPut(::grpc::ClientContext* context, const ::storagequery::PutRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::PutResponse>>(AsyncPutRaw(context, request, cq));
    }
    virtual ::grpc::Status CPut(::grpc::ClientContext* context, const ::storagequery::CPutRequest& request, ::storagequery::CPutResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::CPutResponse>> AsyncCPut(::grpc::ClientContext* context, const ::storagequery::CPutRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::CPutResponse>>(AsyncCPutRaw(context, request, cq));
    }
    virtual ::grpc::Status Delete(::grpc::ClientContext* context, const ::storagequery::DeleteRequest& request, ::storagequery::DeleteResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::DeleteResponse>> AsyncDelete(::grpc::ClientContext* context, const ::storagequery::DeleteRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::DeleteResponse>>(AsyncDeleteRaw(context, request, cq));
    }
    virtual ::grpc::Status Migrate(::grpc::ClientContext* context, const ::storagequery::MigrateRequest& request, ::storagequery::MigrateResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::MigrateResponse>> AsyncMigrate(::grpc::ClientContext* context, const ::storagequery::MigrateRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::MigrateResponse>>(AsyncMigrateRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::GetResponse>* AsyncGetRaw(::grpc::ClientContext* context, const ::storagequery::GetRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::PutResponse>* AsyncPutRaw(::grpc::ClientContext* context, const ::storagequery::PutRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::CPutResponse>* AsyncCPutRaw(::grpc::ClientContext* context, const ::storagequery::CPutRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::DeleteResponse>* AsyncDeleteRaw(::grpc::ClientContext* context, const ::storagequery::DeleteRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::storagequery::MigrateResponse>* AsyncMigrateRaw(::grpc::ClientContext* context, const ::storagequery::MigrateRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub GRPC_FINAL : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status Get(::grpc::ClientContext* context, const ::storagequery::GetRequest& request, ::storagequery::GetResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::GetResponse>> AsyncGet(::grpc::ClientContext* context, const ::storagequery::GetRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::GetResponse>>(AsyncGetRaw(context, request, cq));
    }
    ::grpc::Status Put(::grpc::ClientContext* context, const ::storagequery::PutRequest& request, ::storagequery::PutResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::PutResponse>> AsyncPut(::grpc::ClientContext* context, const ::storagequery::PutRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::PutResponse>>(AsyncPutRaw(context, request, cq));
    }
    ::grpc::Status CPut(::grpc::ClientContext* context, const ::storagequery::CPutRequest& request, ::storagequery::CPutResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::CPutResponse>> AsyncCPut(::grpc::ClientContext* context, const ::storagequery::CPutRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::CPutResponse>>(AsyncCPutRaw(context, request, cq));
    }
    ::grpc::Status Delete(::grpc::ClientContext* context, const ::storagequery::DeleteRequest& request, ::storagequery::DeleteResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::DeleteResponse>> AsyncDelete(::grpc::ClientContext* context, const ::storagequery::DeleteRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::DeleteResponse>>(AsyncDeleteRaw(context, request, cq));
    }
    ::grpc::Status Migrate(::grpc::ClientContext* context, const ::storagequery::MigrateRequest& request, ::storagequery::MigrateResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::MigrateResponse>> AsyncMigrate(::grpc::ClientContext* context, const ::storagequery::MigrateRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::storagequery::MigrateResponse>>(AsyncMigrateRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< ::storagequery::GetResponse>* AsyncGetRaw(::grpc::ClientContext* context, const ::storagequery::GetRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::storagequery::PutResponse>* AsyncPutRaw(::grpc::ClientContext* context, const ::storagequery::PutRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::storagequery::CPutResponse>* AsyncCPutRaw(::grpc::ClientContext* context, const ::storagequery::CPutRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::storagequery::DeleteResponse>* AsyncDeleteRaw(::grpc::ClientContext* context, const ::storagequery::DeleteRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::storagequery::MigrateResponse>* AsyncMigrateRaw(::grpc::ClientContext* context, const ::storagequery::MigrateRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    const ::grpc::RpcMethod rpcmethod_Get_;
    const ::grpc::RpcMethod rpcmethod_Put_;
    const ::grpc::RpcMethod rpcmethod_CPut_;
    const ::grpc::RpcMethod rpcmethod_Delete_;
    const ::grpc::RpcMethod rpcmethod_Migrate_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status Get(::grpc::ServerContext* context, const ::storagequery::GetRequest* request, ::storagequery::GetResponse* response);
    virtual ::grpc::Status Put(::grpc::ServerContext* context, const ::storagequery::PutRequest* request, ::storagequery::PutResponse* response);
    virtual ::grpc::Status CPut(::grpc::ServerContext* context, const ::storagequery::CPutRequest* request, ::storagequery::CPutResponse* response);
    virtual ::grpc::Status Delete(::grpc::ServerContext* context, const ::storagequery::DeleteRequest* request, ::storagequery::DeleteResponse* response);
    virtual ::grpc::Status Migrate(::grpc::ServerContext* context, const ::storagequery::MigrateRequest* request, ::storagequery::MigrateResponse* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_Get : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Get() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Get() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Get(::grpc::ServerContext* context, const ::storagequery::GetRequest* request, ::storagequery::GetResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGet(::grpc::ServerContext* context, ::storagequery::GetRequest* request, ::grpc::ServerAsyncResponseWriter< ::storagequery::GetResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Put : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Put() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_Put() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Put(::grpc::ServerContext* context, const ::storagequery::PutRequest* request, ::storagequery::PutResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestPut(::grpc::ServerContext* context, ::storagequery::PutRequest* request, ::grpc::ServerAsyncResponseWriter< ::storagequery::PutResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_CPut : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_CPut() {
      ::grpc::Service::MarkMethodAsync(2);
    }
    ~WithAsyncMethod_CPut() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CPut(::grpc::ServerContext* context, const ::storagequery::CPutRequest* request, ::storagequery::CPutResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCPut(::grpc::ServerContext* context, ::storagequery::CPutRequest* request, ::grpc::ServerAsyncResponseWriter< ::storagequery::CPutResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(2, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Delete : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Delete() {
      ::grpc::Service::MarkMethodAsync(3);
    }
    ~WithAsyncMethod_Delete() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Delete(::grpc::ServerContext* context, const ::storagequery::DeleteRequest* request, ::storagequery::DeleteResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestDelete(::grpc::ServerContext* context, ::storagequery::DeleteRequest* request, ::grpc::ServerAsyncResponseWriter< ::storagequery::DeleteResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(3, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Migrate : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Migrate() {
      ::grpc::Service::MarkMethodAsync(4);
    }
    ~WithAsyncMethod_Migrate() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Migrate(::grpc::ServerContext* context, const ::storagequery::MigrateRequest* request, ::storagequery::MigrateResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestMigrate(::grpc::ServerContext* context, ::storagequery::MigrateRequest* request, ::grpc::ServerAsyncResponseWriter< ::storagequery::MigrateResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(4, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Get<WithAsyncMethod_Put<WithAsyncMethod_CPut<WithAsyncMethod_Delete<WithAsyncMethod_Migrate<Service > > > > > AsyncService;
  template <class BaseClass>
  class WithGenericMethod_Get : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Get() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Get() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Get(::grpc::ServerContext* context, const ::storagequery::GetRequest* request, ::storagequery::GetResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Put : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Put() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_Put() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Put(::grpc::ServerContext* context, const ::storagequery::PutRequest* request, ::storagequery::PutResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_CPut : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_CPut() {
      ::grpc::Service::MarkMethodGeneric(2);
    }
    ~WithGenericMethod_CPut() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CPut(::grpc::ServerContext* context, const ::storagequery::CPutRequest* request, ::storagequery::CPutResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Delete : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Delete() {
      ::grpc::Service::MarkMethodGeneric(3);
    }
    ~WithGenericMethod_Delete() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Delete(::grpc::ServerContext* context, const ::storagequery::DeleteRequest* request, ::storagequery::DeleteResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Migrate : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Migrate() {
      ::grpc::Service::MarkMethodGeneric(4);
    }
    ~WithGenericMethod_Migrate() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Migrate(::grpc::ServerContext* context, const ::storagequery::MigrateRequest* request, ::storagequery::MigrateResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
};

}  // namespace storagequery


#endif  // GRPC_storage_5fquery_2eproto__INCLUDED
