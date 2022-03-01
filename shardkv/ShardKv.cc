//
// Created by swagger on 2022/2/27.
//


#include "ShardKv.h"
#include "Engine.h"
#include "brpc/server.h"
#include "TransactionContext.h"
#include "../rpc/global.h"


// for debug
#include <iostream>

using std::cout;
using std::endl;


void ShardKv::BEGIN(::google::protobuf::RpcController* c, const ::BeginArgs* args, ::BeginReply* reply,
                    ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  //brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);
  std::shared_ptr<TransactionContext> t(new TransactionContext);
  mutex_.lock();
  t->t_id = args->tid();
  Tmap_.insert(TMapRecord(t->t_id, t));
  mutex_.unlock();

  Engine::BEGIN(t);
  reply->set_err(Prepare_OK);
}


void ShardKv::END(::google::protobuf::RpcController* c, const ::EndArgs* args, ::EndReply* reply,
                  ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);

  mutex_.lock();
  auto iter = Tmap_.find(args->tid());
  if ( iter == Tmap_.end()) {
    reply->set_err(Prepare_NotInit);
    mutex_.unlock();
    return;
  }
  TransactionContextPtr Tctx = iter->second;
  mutex_.unlock();
  bool ret = Engine::END(Tctx);

  mutex_.lock();
  Tmap_.erase(Tctx->t_id);
  mutex_.unlock();
  if ( ret ) {
    reply->set_err(Prepare_OK);
  }else {
    reply->set_err(Prepare_Failed);
  }
}


void ShardKv::PrepareRead(::google::protobuf::RpcController *c, const ::ReadArgs *args,
                          ::ReadReply *reply, ::google::protobuf::Closure *done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller* cntl = static_cast<brpc::Controller*>(c);


  cout << "PrepareRead called" << endl;

  mutex_.lock();
  auto iter = Tmap_.find(args->tid());
  if ( iter == Tmap_.end()) {
    reply->set_err(Prepare_NotInit);
    mutex_.unlock();
    return;
  }
  TransactionContextPtr Tctx = iter->second;
  mutex_.unlock();

  std::string key = args->key();
  cout << "key: " << key << endl;
  std::string value;
  State s = Engine::read(Tctx, key, value);
  if ( s == KeyNotExist ) {
    reply->set_err(Prepare_Failed);
    reply->set_value("Err"); // TODO:
    Engine::ABORT(Tctx);
    return;
  }

  if ( s == LockFailed ) {
    reply->set_err(Prepare_Failed);
    reply->set_value("Err"); // TODO:
    Engine::ABORT(Tctx);
    return;
  }
  reply->set_err(Prepare_OK);
  reply->set_value(value);
}


void ShardKv::PrepareWrite(::google::protobuf::RpcController* c, const ::WriteArgs* args, ::WriteReply* reply,
                           ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);

  mutex_.lock();
  auto iter = Tmap_.find(args->tid());
  if ( iter == Tmap_.end()) {
    reply->set_err(Prepare_NotInit);
    mutex_.unlock();
    return;
  }
  TransactionContextPtr Tctx = iter->second;
  mutex_.unlock();

  std::string key = args->key();
  std::string value = args->value();

  State s = Engine::write(Tctx, key, value);

  if ( s == LockFailed ) {
    Engine::ABORT(Tctx);
    reply->set_err(Prepare_Failed);
    return;
  }
  reply->set_err(Prepare_OK);

}


