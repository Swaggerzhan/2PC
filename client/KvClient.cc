//
// Created by swagger on 2022/2/27.
//
#include "KvClient.h"
#include "brpc/log.h"
#include "../rpc/global.h"


KvClient::KvClient(std::string host)
: stub_(nullptr)
, Tid_(-1)
{
  options_.protocol = "baidu_std";
  options_.connection_type = "";
  options_.timeout_ms = 1000;
  options_.max_retry = 3;

  if ( channel_.Init(host.c_str(), "", &options_) != 0 ) {
    LOG(ERROR)  << "Faid to init channel";
    return;
  }

  stub_ = new KvServerBase_Stub(&channel_);
}


bool KvClient::BEGIN() {
  if ( stub_ == nullptr )
    return false;

  BeginArgs args;
  BeginReply reply;
  brpc::Controller cntl;

  stub_->BEGIN(&cntl, &args, &reply, nullptr);
  if ( !cntl.Failed() ) {
    if ( reply.err() == RPC_OK ) {
      Tid_ = reply.tid();
      LOG(INFO) << "BEGIN latency: " << cntl.latency_us() << " us";
      return true;
    }else {
      return false;
    }
  }
  return false;
}


bool KvClient::END() {
  if ( stub_ == nullptr || Tid_ == -1 )
    return false;

  EndArgs args;
  EndReply reply;
  brpc::Controller cntl;

  args.set_tid(Tid_);

  stub_->END(&cntl, &args, &reply, nullptr);

  if ( !cntl.Failed() ) {
    if ( reply.err() == RPC_OK ) {
      return true;
    }else {
      return false;
    }
  }
  return false;

}

bool KvClient::read(std::string& key, std::string& value) {
  if ( stub_ == nullptr || Tid_ == -1 )
    return false;

  GetArgs args;
  GetReply reply;
  brpc::Controller cntl;

  args.set_tid(Tid_);
  args.set_key(key);

  stub_->Get(&cntl, &args, &reply, nullptr);
  if ( !cntl.Failed() ) {
    if ( reply.err() == RPC_OK ) {
      value = reply.value();
      return true;
    }else {
      return false;
    }
  }
  return false;
}


bool KvClient::write(std::string& key, std::string& value) {
  if ( stub_ == nullptr || Tid_ == -1 )
    return false;

  PutArgs args;
  PutReply reply;
  brpc::Controller cntl;

  args.set_tid(Tid_);
  args.set_key(key);
  args.set_value(value);

  stub_->Put(&cntl, &args, &reply, nullptr);
  if ( !cntl.Failed() ) {
    if (reply.err() == RPC_OK ) {
      value = reply.value();
      return true;
    }else {
      return false;
    }
  }
  return false;
}


