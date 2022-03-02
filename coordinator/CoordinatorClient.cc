//
// Created by swagger on 2022/3/2.
//
#include "CoordinatorClient.h"
#include "brpc/log.h"
#include "../util/global.h"


// for debug
#include <iostream>
using std::cout;
using std::endl;


CoordinatorClient::CoordinatorClient(std::string host)
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
  stub_ = new TransactionCoordinatorBase_Stub(&channel_);
}


RpcState CoordinatorClient::BEGIN() {
  if (stub_ == nullptr){
    return Transaction_NotInit;
  }
  BeginArgs args;
  BeginReply reply;
  brpc::Controller cntl;

  stub_->BEGIN(&cntl, &args, &reply, nullptr);
  if ( cntl.Failed() ) {
    return Transaction_NetworkFailed;
  }
  if ( reply.err() == Transaction_OK ) {
    Tid_ = reply.tid();
    return Transaction_OK;
  }else {
    return Transaction_Failed;
  }
}


RpcState CoordinatorClient::END() {
  if ( stub_ == nullptr || Tid_ == -1)
    return Transaction_NotInit;

  EndArgs args;
  EndReply reply;
  brpc::Controller cntl;

  args.set_tid(Tid_);

  stub_->END(&cntl, &args, &reply, nullptr);

  if ( cntl.Failed() ) {
    return Transaction_NetworkFailed;
  }

  if ( reply.err() == Transaction_OK ) {
    return Transaction_OK;
  }

  if ( reply.err() == Transaction_NotInit ) {
    return Transaction_NotInit;
  }
  return Transaction_Failed;
}

RpcState CoordinatorClient::ABORT() {
  if ( stub_ == nullptr || Tid_ == -1)
    return Transaction_NotInit;
  AbortArgs args;
  AbortReply reply;
  brpc::Controller cntl;

  args.set_tid(Tid_);

  stub_->ABORT(&cntl, &args, &reply, nullptr);
  if ( cntl.Failed() ) {
    return Transaction_NetworkFailed;
  }

  if ( reply.err() == Transaction_OK ) {
    return Transaction_OK;
  }
  return Transaction_Failed;
}



RpcState CoordinatorClient::Read(std::string& key, std::string& value) {
  if ( stub_ == nullptr || Tid_ == -1)  {
    return Transaction_NotInit;
  }

  brpc::Controller cntl;
  ReadArgs args;
  ReadReply reply;

  args.set_tid(Tid_);
  args.set_key(key);

  stub_->READ(&cntl, &args, &reply, nullptr);

  if ( cntl.Failed() ) {
    return Transaction_NetworkFailed;
  }

  if ( reply.err() == Transaction_OK ) {
    value = reply.value();
    return Transaction_OK;
  }

  if ( reply.err() == Transaction_NotInit ) {
    return Transaction_NotInit;
  }

  if ( reply.err() == Transaction_KeyNotExist) {
    return Transaction_KeyNotExist;
  }

  return Transaction_Failed;
}


RpcState CoordinatorClient::Write(std::string& key, std::string& value) {
  if ( stub_ == nullptr || Tid_ == -1 ){
    return Transaction_NotInit;
  }

  brpc::Controller cntl;
  WriteArgs args;
  WriteReply reply;

  args.set_tid(Tid_);
  args.set_key(key);
  args.set_value(value);

  stub_->WRITE(&cntl, &args, &reply, nullptr);

  if ( cntl.Failed() ) {
    return Transaction_NetworkFailed;
  }

  if ( reply.err() == Transaction_OK ) {
    return Transaction_OK;
  }

  if ( reply.err() == Transaction_NotInit ){
    return Transaction_NotInit;
  }

  return Transaction_Failed;
}






