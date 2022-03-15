//
// Created by swagger on 2022/2/28.
//
#include "ShardKvClient.h"
#include "brpc/log.h"
#include "../util/global.h"


// for debug
#include <iostream>
using std::cout;
using std::endl;


ShardKvClient::ShardKvClient(std::string host)
        : stub_(nullptr)
        , Tid_(-1)
{
  options_.protocol = "baidu_std";
  options_.connection_type = "";
  options_.timeout_ms = 6000;
  options_.max_retry = 3;

  if ( channel_.Init(host.c_str(), "", &options_) != 0 ) {
    LOG(ERROR)  << "Faid to init channel";
    return;
  }
  LOG(INFO) << "connect to: " << host;
  stub_ = new ShardKvBase_Stub(&channel_);
}


RpcState ShardKvClient::BEGIN(int tid) {
  if (stub_ == nullptr){
    return Prepare_NotInit;
  }
  ShardBeginArgs args;
  ShardBeginReply reply;
  brpc::Controller cntl;

  args.set_tid(tid);

  stub_->BEGIN(&cntl, &args, &reply, nullptr);
  if ( cntl.Failed() ) {
    return Prepare_ConnectFail;
  }
  if ( reply.err() == Prepare_OK ) {
    return Prepare_OK;
  }else {
    return Prepare_Failed;
  }
}


RpcState ShardKvClient::END(int tid, int commitID) {
  if ( stub_ == nullptr )
    return Prepare_NotInit;

  ShardEndArgs args;
  ShardEndReply reply;
  brpc::Controller cntl;

  args.set_tid(tid);
  args.set_commitid(commitID);

  stub_->END(&cntl, &args, &reply, nullptr);

  if ( cntl.Failed() ) {
    return Prepare_ConnectFail;
  }

  if ( reply.err() == Prepare_OK ) {
    return Prepare_OK;
  }

  if ( reply.err() == Prepare_NotInit ) {
    return Prepare_NotInit;
  }
  return Prepare_Failed;
}

RpcState ShardKvClient::ABORT(int tid) {
  if ( stub_ == nullptr )
    return Prepare_NotInit;
  ShardAbortArgs args;
  ShardAbortReply reply;
  brpc::Controller cntl;

  args.set_tid(tid);

  stub_->ABORT(&cntl, &args, &reply, nullptr);
  if ( cntl.Failed() ) {
    return Prepare_ConnectFail;
  }

  if ( reply.err() == Prepare_OK ) {
    return Prepare_OK;
  }
  return Prepare_Failed;
}



RpcState ShardKvClient::PrepareRead(int tid, std::string& key, std::string& value) {
  if ( stub_ == nullptr )  {
    return Prepare_NotInit;
  }

  brpc::Controller cntl;
  ShardReadArgs args;
  ShardReadReply reply;

  args.set_tid(tid);
  args.set_key(key);

  stub_->PrepareRead(&cntl, &args, &reply, nullptr);

  if ( cntl.Failed() ) {
    return Prepare_ConnectFail;
  }

  if ( reply.err() == Prepare_OK ) {
    value = reply.value();
    return Prepare_OK;
  }

  if ( reply.err() == Prepare_NotInit ) {
    return Prepare_NotInit;
  }

  if ( reply.err() == Prepare_KeyNotExist) {
    return Prepare_KeyNotExist;
  }

  return Prepare_Failed;
}


RpcState ShardKvClient::PrepareWrite(int tid, std::string& key, std::string& value) {
  if ( stub_ == nullptr ){
    return Prepare_NotInit;
  }

  brpc::Controller cntl;
  ShardWriteArgs args;
  ShardWriteReply reply;

  args.set_tid(tid);
  args.set_key(key);
  args.set_value(value);

  stub_->PrepareWrite(&cntl, &args, &reply, nullptr);

  if ( cntl.Failed() ) {
    return Prepare_ConnectFail;
  }

  if ( reply.err() == Prepare_OK ) {
    return Prepare_OK;
  }

  if ( reply.err() == Prepare_NotInit ) {
    return Prepare_NotInit;
  }

  return Prepare_Failed;
}

































