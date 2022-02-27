//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_GLOBAL_H
#define INC_2PC_GLOBAL_H



enum RpcState{
  RPC_OK = 1,
  RPC_TransactionNotInit = 2,
  RPC_KeyNotExist = 3,
  RPC_WriteFailed = 4,
};

#endif //INC_2PC_GLOBAL_H
