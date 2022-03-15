//
// Created by swagger on 2022/2/28.
//

#ifndef INC_2PC_SHARDKVCLIENT_H
#define INC_2PC_SHARDKVCLIENT_H



#include "../proto/shardkv.pb.h"
#include "brpc/channel.h"
#include "gflags/gflags.h"
#include "../util/global.h"


class ShardKvClient {
public:

  // 0.0.0.0:8888
  ShardKvClient(std::string);

  ~ShardKvClient()=default;

  // tid
  RpcState BEGIN(int);

  RpcState PrepareRead(int, std::string&, std::string&);
  RpcState PrepareWrite(int, std::string&, std::string&);

  // tid
  RpcState END(int, int);

  RpcState ABORT(int);


private:
  brpc::Channel channel_;
  brpc::ChannelOptions options_;
  ShardKvBase_Stub* stub_;
  int Tid_;

};






#endif //INC_2PC_SHARDKVCLIENT_H
