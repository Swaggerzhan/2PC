//
// Created by swagger on 2022/2/27.
//


#include "ShardKv.h"
#include "Engine.h"
#include "brpc/server.h"
#include "TransactionContext.h"
#include "../util/global.h"


// for debug
#include <iostream>

using std::cout;
using std::endl;


void ShardKv::BEGIN(::google::protobuf::RpcController* c, const ::ShardBeginArgs* args, ::ShardBeginReply* reply,
                    ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  //brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);
  TransactionContext* Tctx = new TransactionContext;
  mutex_.lock();
  Tctx->id = args->tid();
  Tmap_.insert(TMapRecord(Tctx->id, Tctx));
  mutex_.unlock();

  reply->set_err(Prepare_OK);
}


void ShardKv::END(::google::protobuf::RpcController* c, const ::ShardEndArgs* args, ::ShardEndReply* reply,
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
  TransactionContext* Tctx = iter->second;
  mutex_.unlock();

  if ( Tctx->invalid )  {
    reply->set_err(Prepare_Failed);
    return;
  }
  Tctx->commit_id = args->commitid();
  State ret = Engine::commit(Tctx);

  mutex_.lock();
  Tmap_.erase(Tctx->id);
  mutex_.unlock();
  if ( ret == Success ) {
    reply->set_err(Prepare_OK);
  }else { // TODO: when fail happened
    reply->set_err(Prepare_Failed);
    Tctx->invalid = true;
  }
}


void ShardKv::PrepareRead(::google::protobuf::RpcController *c, const ::ShardReadArgs *args,
                          ::ShardReadReply *reply, ::google::protobuf::Closure *done) {
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
  TransactionContext* Tctx = iter->second;
  mutex_.unlock();

  // Transaction already failed
  if ( Tctx->invalid )  {
    reply->set_err(Prepare_Failed);
    return;
  }

  std::string key = args->key();
  cout << "key: " << key << endl;
  std::string value;
  State s = Engine::read(Tctx, key, value);
  // TODO: ShardKV无权利移除失败的事务
  if ( s == KeyNotExist ) {
    reply->set_err(Prepare_KeyNotExist);
    Tctx->invalid = true;
    //Engine::ABORT(Tctx);
    //removeContext(args->tid());
    return;
  }

  if ( s == LockFailed ) {
    reply->set_err(Prepare_Failed);
    Tctx->invalid = true;
    //Engine::ABORT(Tctx);
    //removeContext(args->tid());
    return;
  }
  reply->set_err(Prepare_OK);
  reply->set_value(value);
}


void ShardKv::PrepareWrite(::google::protobuf::RpcController* c, const ::ShardWriteArgs* args, ::ShardWriteReply* reply,
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
  TransactionContext* Tctx = iter->second;
  mutex_.unlock();

  if ( Tctx->invalid ) {
    reply->set_err(Prepare_Failed);
    return;
  }

  std::string key = args->key();
  std::string value = args->value();

  State s = Engine::write(Tctx, key, value);

  if ( s == LockFailed ) {
    Tctx->invalid = true;
    reply->set_err(Prepare_Failed);
    return;
  }
  reply->set_err(Prepare_OK);

}

void ShardKv::ABORT(::google::protobuf::RpcController*, const ::ShardAbortArgs* args,
                    ::ShardAbortReply* reply,
                    ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);

  mutex_.lock();
  auto iter = Tmap_.find(args->tid());
  if ( iter == Tmap_.end() ) {
    mutex_.unlock();
    reply->set_err(Prepare_NotInit);
    return;
  }
  TransactionContext* Tctx = iter->second;
  Tmap_.erase(args->tid());
  mutex_.unlock();

  Engine::abort(Tctx);
  reply->set_err(Prepare_OK);
}


void ShardKv::removeContext(int tid) {
  std::unique_lock<std::mutex> lockGuard(mutex_);
  Tmap_.erase(tid);
}

