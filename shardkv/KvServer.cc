//
// Created by swagger on 2022/2/27.
//


#include "KvServer.h"
#include "brpc/server.h"
#include "Engine.h"
#include "TransactionContext.h"


void KvServer::BEGIN(::google::protobuf::RpcController* c, const ::BeginArgs* args, ::BeginReply* reply,
                     ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);
  TransactionContext* t = new TransactionContext;
  mutex_.lock();
  Tmap_.insert(TMapRecord(t->t_id, t));
  mutex_.unlock();

  Engine::BEGIN(t);

  reply->set_err(RPC_OK);
  reply->set_tid(t->t_id);
}


void KvServer::END(::google::protobuf::RpcController* c, const ::EndArgs* args, ::EndReply* reply,
                   ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);

  mutex_.lock();
  auto iter = Tmap_.find(args->tid());
  if (iter == Tmap_.end()) {
    reply->set_err(RPC_TransactionNotInit);
    mutex_.unlock();
    return;
  }
  Engine::END(args->tid());
  Tmap_.erase(args->tid());
  mutex_.unlock();
  reply->set_err(RPC_OK);
}



void KvServer::Get(::google::protobuf::RpcController* c, const ::GetArgs* args, ::GetReply* reply,
                       ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);

  mutex_.lock();
  auto iter = Tmap_.find(args->tid());
  if (iter == Tmap_.end()) {
   reply->set_err(RPC_TransactionNotInit);
    mutex_.unlock();
    return;
  }
  mutex_.unlock();
  std::string key = args->key();
  std::string value;
  Engine::read(args->tid(), key, value);
  reply->set_value(value);
  reply->set_err(RPC_OK);
}


void KvServer::Put(::google::protobuf::RpcController* c, const ::PutArgs* args, ::PutReply* reply,
                   ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);
  mutex_.lock();
  auto iter = Tmap_.find(args->tid());
  if (iter == Tmap_.end()) {
    reply->set_err(RPC_TransactionNotInit);
    mutex_.unlock();
    return;
  }
  mutex_.unlock();
  std::string key = args->key();
  std::string value = args->value();

  State s = Engine::write(args->tid(), key, value);
  if ( s != Success ) {
    reply->set_err(RPC_WriteFailed);
    return;
  }
  reply->set_err(RPC_OK);
  reply->set_value("OK"); // TODO: put不需要返回value, 返回旧的？
}






