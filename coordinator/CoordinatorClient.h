//
// Created by swagger on 2022/3/2.
//

#ifndef INC_2PC_COORDINATORCLIENT_H
#define INC_2PC_COORDINATORCLIENT_H


#include "../proto/transaction.pb.h"
#include "brpc/channel.h"
#include "gflags/gflags.h"
#include "../util/global.h"


class CoordinatorClient {
public:

  // 0.0.0.0:8888
  CoordinatorClient(std::string);

  ~CoordinatorClient()=default;

  // tid
  RpcState BEGIN();

  RpcState Read(std::string&, std::string&);
  RpcState Write(std::string&, std::string&);

  // tid
  RpcState END();

  RpcState ABORT();


private:
  brpc::Channel channel_;
  brpc::ChannelOptions options_;
  TransactionCoordinatorBase_Stub* stub_;
  int Tid_;

};








#endif //INC_2PC_COORDINATORCLIENT_H
