//
// Created by swagger on 2022/2/27.
//

#include "TransactionCoordinator.h"
#include "brpc/server.h"
#include "../shardkv/TransactionController.h"
#include "../util/MurmurHash2.h"

#include <iostream>

using std::string;
using std::cout;
using std::endl;


void CoordinatorContext::record(int shard) {
  for (auto i : usedShard ) {
    if ( i == shard )
      return;
  }
  usedShard.push_back(shard);
}


TransactionCoordinator::TransactionCoordinator(std::vector<string> shardKvs)
: kRound_(shardKvs.size())
, kSeed_(10)
{
  int index = 0;
  for (auto host : shardKvs ) {
    shardKv_.emplace(
            std::pair<int, ShardKvClientPtr>(
                  index,  std::make_shared<ShardKvClient>(host)
                    ));
    index ++;
  }
}


void TransactionCoordinator::BEGIN(::google::protobuf::RpcController* c, const ::BeginArgs* args,
                                   ::BeginReply* reply,
                                   ::google::protobuf::Closure* done) {

  brpc::ClosureGuard done_guard(done);
  //brpc::Controller* ctnl = static_cast<brpc::Controller*>(c);

  int tid = TransactionID::get();
  reply->set_tid(tid);
  reply->set_err(Transaction_OK);
  addCoordinator(tid);
}


void TransactionCoordinator::END(::google::protobuf::RpcController* c, const ::EndArgs* args,
                                 ::EndReply* reply,
                                 ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);

  CoordinatorContextPtr contextPtr;
  RpcState ret = getCoordinatorContext(args->tid(), contextPtr);
  if ( ret == Transaction_NotInit ) {
    reply->set_err(Transaction_NotInit);
    return;
  }

  // 取出后直接删除
  delCoordinatorContext(args->tid());

  std::vector<ShardKvClientPtr> tmp;
  mutex_.lock();
  for (int i : contextPtr->usedShard) {
    auto iter = shardKv_.find(i);
    if ( iter != shardKv_.end() ) {
      tmp.push_back(iter->second);
    }
  }
  mutex_.unlock();

  bool ok = true;
  for (auto i : tmp) {
    // TODO: 使用并发？
    if (i->END(args->tid()) != Prepare_OK ) {
      ok = false;
    }
  }
  if ( ok ) {
    reply->set_err(Transaction_OK);
    return;
  }
  reply->set_err(Transaction_Failed);

}


void TransactionCoordinator::READ(::google::protobuf::RpcController *, const ::ReadArgs* args,
                                  ::ReadReply* reply,
                                  ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);

  string err_value = "Err Value";
  reply->set_value(err_value);

  CoordinatorContextPtr contextPtr;
  RpcState ret = getCoordinatorContext(args->tid(), contextPtr);

  cout << "READ called" << endl;

  if ( ret == Transaction_NotInit ) {
    reply->set_err(Transaction_NotInit);
    cout << State2Str(ret) << endl;
    return;
  }
  ShardKvClientPtr shardKvClientPtr;
  int shard;
  if ( (shard = getShardKvClient(args->key(), shardKvClientPtr)) == -1 ) {
    // TODO: handle on error   回滚
    errorAndAbort(args->tid());
    cout << "shard = " << shard << endl;
    return;
  }
  string key = args->key();
  string value;
  RpcState s = shardKvClientPtr->PrepareRead(args->tid(), key, value);

  if ( s == Prepare_NotInit ) {
    s = shardKvClientPtr->BEGIN(args->tid());
    if ( s != Prepare_OK ) {

      if (s == Prepare_ConnectFail) {
        reply->set_err(Transaction_NetworkFailed);
      }else {
        reply->set_err(Transaction_Failed);
      }
      // TODO: 回滚
      errorAndAbort(args->tid());
      cout << State2Str(s) << endl;
      return;
    }
    s = shardKvClientPtr->PrepareRead(args->tid(), key, value);
  }

  if ( s == Prepare_OK ) {
    contextPtr->record(shard);
    reply->set_err(Transaction_OK);
    // TODO: what?
    reply->set_value(value);
    cout << State2Str(s) << endl;
    return;
  }

  if ( s == Prepare_KeyNotExist ) {
    reply->set_err(Transaction_KeyNotExist);
    errorAndAbort(args->tid());
    cout << State2Str(s) << endl;
    return;
  }


  reply->set_err(Transaction_Failed);
  // TODO: 发生错误 回滚
  errorAndAbort(args->tid());
  cout << State2Str(s) << endl;
}


void TransactionCoordinator::WRITE(::google::protobuf::RpcController *, const ::WriteArgs* args,
                                   ::WriteReply* reply,
                                   ::google::protobuf::Closure* done) {
  brpc::ClosureGuard done_guard(done);

  LOG(INFO) << "WRITE, begin";

  CoordinatorContextPtr contextPtr;
  RpcState s = getCoordinatorContext(args->tid(), contextPtr);
  if ( s == Transaction_NotInit ) {
    reply->set_err(Transaction_NotInit);
    LOG(INFO) << "Transaction_NotInit";
    return;
  }
  LOG(INFO) << "WRITE 159";
  ShardKvClientPtr shardKvClientPtr;
  int shard;
  if ( (shard = getShardKvClient(args->key(), shardKvClientPtr)) == -1 ) {
    // TODO: 回滚
    reply->set_err(Transaction_Failed);
    LOG(INFO) << "Transaction_Failed, shard == -1";
    return;
  }
  LOG(INFO) << "WRITE 168";
  string key = args->key();
  string value = args->value();
  s = shardKvClientPtr->PrepareWrite(args->tid(), key, value);

  if ( s == Prepare_NotInit ) {
    s = shardKvClientPtr->BEGIN(args->tid());
    if ( s != Prepare_OK ) {
      reply->set_err(Transaction_Failed);
      // TODO: 回滚
      errorAndAbort(args->tid());
      LOG(INFO) << "Shard.BEGIN NOT OK";
      return;
    }
    LOG(INFO) << "Shard.BEGIN OK";
    cout << "shard.BEGIN OK" << endl;
    s = shardKvClientPtr->PrepareWrite(args->tid(), key, value);
    cout << s << endl;
  }

  if ( s == Prepare_OK ) {
    reply->set_err(Transaction_OK);
    contextPtr->record(shard);
    cout << 4 << endl;
    return;
  }

  if ( s == Prepare_ConnectFail ) {
    reply->set_err(Transaction_NetworkFailed);
    errorAndAbort(args->tid());
    return;
  }

  reply->set_err(Transaction_Failed);
  // TODO: 回滚
  errorAndAbort(args->tid());
  cout << State2Str(s) << endl;
}


void TransactionCoordinator::errorAndAbort(int tid) {
  CoordinatorContextPtr ptr;
  RpcState s = getCoordinatorContext(tid, ptr);
  if ( s == Transaction_NotInit )
    return;
  delCoordinatorContext(tid);
  std::vector<ShardKvClientPtr> tmp;
  mutex_.lock();
  for (int i : ptr->usedShard) {
    auto iter = shardKv_.find(i);
    if ( iter != shardKv_.end() ) {
      tmp.push_back(iter->second);
    }
  }
  mutex_.unlock();

  for (auto i : tmp) {
    // TODO: 使用并发？
    i->ABORT(tid);
  }
}


void TransactionCoordinator::addCoordinator(int tid) {
  std::unique_lock<std::mutex> lock_guard(latch_);
  CoordinatorContextPtr ptr(new CoordinatorContext);
  coordinatorsCtx_.emplace(CoordinatorRecord(tid, ptr));
}

RpcState TransactionCoordinator::getCoordinatorContext(int tid, CoordinatorContextPtr& ptr) {
  std::unique_lock<std::mutex> lock_guard(latch_);
  auto iter = coordinatorsCtx_.find(tid);
  if ( iter == coordinatorsCtx_.end())
    return Transaction_NotInit;
  ptr = iter->second;
  return Transaction_OK;
}

void TransactionCoordinator::delCoordinatorContext(int tid) {
  std::unique_lock<std::mutex> lock_guard(latch_);
  coordinatorsCtx_.erase(tid);
}

// -1 for error
// >= 0 for the shard
int TransactionCoordinator::getShardKvClient(std::string key, ShardKvClientPtr& ptr) {
  uint32_t ret = MurmurHash2(key.c_str(), key.length(), kSeed_);
  int shard = ret % kRound_;
  std::unique_lock<std::mutex> lock_guard(mutex_);
  auto iter = shardKv_.find(shard);
  if ( iter == shardKv_.end() ) {
    return -1;
  }
  ptr = iter->second;
  return shard;
}










