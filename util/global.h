//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_GLOBAL_H
#define INC_2PC_GLOBAL_H


#include <string>

enum RpcState{
  RPC_OK = 1,


  Prepare_Failed = -1,
  Prepare_KeyNotExist = 5,
  Prepare_OK = 6,
  Prepare_NotInit = 7,
  Prepare_ConnectFail = 8,


  Transaction_OK = 9,
  Transaction_Failed = -2,
  Transaction_KeyNotExist = 10,
  Transaction_NotInit = 11,
  Transaction_NetworkFailed = 12,

};


std::string State2Str(RpcState);


#endif //INC_2PC_GLOBAL_H
