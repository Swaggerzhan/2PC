//
// Created by swagger on 2022/2/27.
//

#include "TransactionCoordinator.h"
#include "brpc/server.h"
#include "../shardkv/TransactionController.h"
#include "../util/MurmurHash2.h"

using std::string;


TransactionCoordinator::TransactionCoordinator()
: kRound_(2)
, kSeed_(10)
{}


void TransactionCoordinator::BEGIN(::google::protobuf::RpcController* c, const ::BeginArgs* args,
                                   ::BeginReply* reply,
                                   ::google::protobuf::Closure* done) {

  brpc::ClosureGuard done_guard(done);
  //brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);

  int tid = TransactionID::get();
  reply->set_tid(tid);
  reply->set_err(Transaction_OK);

  latch_.lock();
  auto p = new std::vector<ShardKvClientPtr>();
  transactions_.emplace(TransactionRecord(tid, p));
  latch_.unlock();

}


void TransactionCoordinator::END(::google::protobuf::RpcController* c, const ::EndArgs* args,
                                 ::EndReply* reply,
                                 ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);
  std::vector<ShardKvClientPtr>* kvClients;

  latch_.lock();
  auto iter = transactions_.find(args->tid());
  if (iter == transactions_.end()) {
    reply->set_err(Transaction_Failed);
    latch_.unlock();
    return;
  }

  kvClients = iter->second;
  latch_.unlock();

  bool isFailed = false;
  while ( !kvClients->empty() ) {
    ShardKvClientPtr c = kvClients->back();
    kvClients->pop_back();
    RpcState s = c->END(args->tid());
    if ( s == Prepare_Failed ) {
      isFailed = true;
    }
  }

  latch_.lock();
  transactions_.erase(args->tid());
  delete kvClients;
  latch_.unlock();



  if (!isFailed) {
    reply->set_err(Transaction_OK);
    return;
  }
  reply->set_err(Transaction_Failed);
}


void TransactionCoordinator::READ(::google::protobuf::RpcController *, const ::ReadArgs* args,
                                  ::ReadReply* reply,
                                  ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);

  latch_.lock();
  auto iter = transactions_.find(args->tid());
  if ( iter == transactions_.end() ) {
    reply->set_err(Transaction_NotInit);
    latch_.unlock();
    return;
  }
  std::vector<ShardKvClientPtr>* pContext = iter->second;
  latch_.unlock();

  string key = args->key();

  uint32_t ret = MurmurHash2(key.c_str(), key.length(), kSeed_);
  int shard = ret % kRound_;

  mutex_.lock();
  auto it = shardKv_.find(shard);
  if (it == shardKv_.end()) { // TODO:发生错误
    latch_.lock();
    delete pContext;
    transactions_.erase(args->tid());
    latch_.unlock();
    return;
  }
  ShardKvClientPtr shardKvClientPtr = iter->second;
  mutex_.unlock();

  string value;
  RpcState s = shardKvClientPtr->PrepareRead(args->tid, key, value);
  if ( s == Prepare_OK ) {
    pContext->push_back(shardKvClientPtr);
    reply->set_err(Transaction_OK);
    // TODO: what?
    // reply->set_value(value);
    return;
  }
  reply->set_err(Transaction_Failed);
  // TODO: 发生错误
}


void TransactionCoordinator::WRITE(::google::protobuf::RpcController *, const ::WriteArgs* args,
                                   ::WriteReply* reply,
                                   ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);

  latch_.lock();
  auto iter = transactions_.find(args->tid());
  if ( iter == transactions_.end() ) {
    latch_.unlock();
    reply->set_err(Transaction_NotInit);
    return;
  }
  std::vector<ShardKvClientPtr>* pContext = iter->second;
  latch_.unlock();

  string key = args->key();
  string value = args->value();

  uint32_t ret = MurmurHash2(key.c_str(), key.length(), kSeed_);
  int shard = ret % kRound_;

  mutex_.lock();
  auto it = shardKv_.find(shard);
  if ( it == shardKv_.end() ) {
    // TODO: 错误处理
    mutex_.unlock();
    return;
  }
  ShardKvClientPtr shardKvClientPtr = iter->second;
  mutex_.unlock();

  RpcState s = shardKvClientPtr->PrepareWrite(args->tid(), key, value);
  if ( s == Prepare_OK ) {
    reply->set_err(Transaction_OK);
    pContext->push_back(shardKvClientPtr);
    return;
  }
  // TODO: 错误处理
}


void TransactionCoordinator::errorAndAbort(int tid, std::vector<ShardKvClientPtr>* p) {
  while ( !p->empty() ) {
    ShardKvClientPtr shard = p->back();
    p->pop_back();
    // TODO:
    p->Abort(tid);
  }
}












