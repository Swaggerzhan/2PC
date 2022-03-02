//
// Created by swagger on 2022/2/27.
//

#include "global.h"


std::string State2Str(RpcState s) {
  switch (s) {
    case Prepare_Failed: {
      return "PrepareFailed";
    }
    case Prepare_NotInit: {
      return "PrepareNotInit";
    }
    case Prepare_ConnectFail: {
      return "PrepareNetworkFailed";
    }
    case Prepare_KeyNotExist: {
      return "Prepare_KeyNotExist";
    }
    case Prepare_OK: {
      return "OK";
    }
    case Transaction_Failed: {
      return "Transaction_Failed";
    }
    case Transaction_OK: {
      return "OK";
    }
    case Transaction_NetworkFailed: {
      return "Transaction_NetworkFailed";
    }
    case Transaction_NotInit: {
      return "Transaction_NotInit";
    }
    case Transaction_KeyNotExist: {
      return "Transaction_KeyNotExist";
    }
    default: {
      return "State Not Found";
    }
  }
}



